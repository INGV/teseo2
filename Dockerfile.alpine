# Docker for GIMP Development Environment based on alpine
# To guarantee gimp-2.8.x use alpine 3.8.4
FROM alpine:3.8.4

LABEL maintainer="Matteo Quintiliani <matteo.quintiliani@ingv.it>"

ENV DEBIAN_FRONTEND=noninteractive
ENV INITRD No
ENV FAKE_CHROOT 1
ENV GROUP_NAME=teseo
ENV USER_NAME=teseo
ENV PASSWORD_USER=teseor
ENV HOMEDIR_USER=/home/${USER_NAME}
ENV BASHRC_USER=${HOMEDIR_USER}/.bashrc_custom_user
ENV DOCKERIGNORED_DIR=/docker_ignored_dir

##################################
# Install necessary packages
##################################
RUN apk add --no-cache \
		procps \
		bash \
		openssh-client \
		openssh \
		openrc \
		git \
		procps \
		sudo \
		binutils \
		gcc \
		g++ \
		gfortran \
		make \
		gdb \
		libc-dev \
		automake \
		autoconf \
		libtool \
		ttf-dejavu \
		vim \
		screen \
		xauth \
		glade3 \
		gimp-dev

##################################
# Configure SSH server
##################################
RUN rc-update add sshd boot
RUN rc-status
RUN touch /run/openrc/softlevel
RUN \
		ssh-keygen -f /etc/ssh/ssh_host_rsa_key -N '' -t rsa \
		&& ssh-keygen -f /etc/ssh/ssh_host_dsa_key -N '' -t dsa \
		&& mkdir -p /var/run/sshd

# Set SSH server X11Forwarding and X11Forwarding options
# Require package 'xauth'
RUN \
		sed -i'.bak' -e 's/^\(.*X11Forwarding.*\)$/#mq \1/' -e 's/^\(.*X11UseLocalhost.*\)$/#mq \1/'  /etc/ssh/sshd_config \
		&& echo "X11Forwarding yes" >> /etc/ssh/sshd_config \
		&& echo "X11UseLocalhost no" >> /etc/ssh/sshd_config

##################################
# Create and set SOFTDIR_USER
##################################
ENV SOFTDIR_USER=/opt/soft
RUN mkdir -p ${SOFTDIR_USER}
WORKDIR ${SOFTDIR_USER}

##################################
# Compile and Install gtkdatabox
##################################
# wget https://netcologne.dl.sourceforge.net/project/gtkdatabox/gtkdatabox/0.9.3.0/gtkdatabox-0.9.3.0.tar.gz
# wget https://netcologne.dl.sourceforge.net/project/gtkdatabox/gtkdatabox/0.9.1.1/gtkdatabox-0.9.1.1.tar.gz
# wget https://netcologne.dl.sourceforge.net/project/gtkdatabox/gtkdatabox/0.9.1.3/gtkdatabox-0.9.1.3.tar.gz
ENV GTKDATABOX_VERSION=0.9.3.1
RUN wget https://netcologne.dl.sourceforge.net/project/gtkdatabox/gtkdatabox/${GTKDATABOX_VERSION}/gtkdatabox-${GTKDATABOX_VERSION}.tar.gz \
		&& tar xvfz gtkdatabox-${GTKDATABOX_VERSION}.tar.gz \
		&& cd gtkdatabox-${GTKDATABOX_VERSION} \
		&& ./configure && make && sudo make install

##################################
# Copy 'teseo2' directories
##################################
# RUN git clone git@gitlab.rm.ingv.it:teseo/teseo2.git
# RUN mkdir -p ${SOFTDIR_USER}/teseo2/teseo-dist
COPY ./gtk-addons ${SOFTDIR_USER}/teseo2/teseo-dist/gtk-addons
COPY ./newuoa ${SOFTDIR_USER}/teseo2/teseo-dist/newuoa
COPY ./teseo-2 ${SOFTDIR_USER}/teseo2/teseo-dist/teseo-2

# ##################################
# # Compile and install teseo2
# ##################################
ENV CFLAGS="-g"
WORKDIR ${SOFTDIR_USER}/teseo2
RUN cd ./teseo-dist/gtk-addons \
		&&  ./configure && make && sudo make install

RUN cd ./teseo-dist/newuoa \
		&& sed -i'.bak' -e "s/glibtoolize/libtoolize/g" bootstrap \
		&& ./bootstrap \
		&&  ./configure && make && sudo make install

RUN cd ./teseo-dist/teseo-2 \
		&& mkdir m4 \
		&&  ./bootstrap \
		&&  ./configure --datadir=/usr/share/gimp/2.0 && make && sudo make install

##################################
# Create user and add to sudo users
##################################
# Add package 'shadow' only for usermod and groupmod
# then remove it because it installs a different version of chpasswd
RUN apk --no-cache add shadow

# Required arguments
ARG ENV_UID=0
ARG ENV_GID=0

RUN echo ENV_UID=${ENV_UID}
RUN echo ENV_GID=${ENV_GID}

RUN \
		if [ ${ENV_UID} -eq 0 ] || [ ${ENV_GID} -eq 0 ]; \
		then \
			echo ""; \
			echo "WARNING: when passing UID or GID equal to zero, new user and/or group are created."; \
			echo "         On Linux, if you run docker image by different UID or GID you could not able to write in docker mount data directory."; \
			echo ""; \
		fi

# Check if GID already exists
RUN cat /etc/group
RUN \
		if [ ${ENV_GID} -eq 0 ]; \
		then \
			addgroup -S ${GROUP_NAME}; \
		elif grep -q -e "[^:][^:]*:[^:][^:]*:${ENV_GID}:.*$" /etc/group; \
		then \
			GROUP_NAME_ALREADY_EXISTS=$(grep  -e "[^:][^:]*:[^:][^:]*:${ENV_GID}:.*$" /etc/group | cut -f 1 -d':'); \
			echo "GID ${ENV_GID} already exists with group name ${GROUP_NAME_ALREADY_EXISTS}"; \
			groupmod -n ${GROUP_NAME} ${GROUP_NAME_ALREADY_EXISTS}; \
		else \
			echo "GID ${ENV_GID} does not exist"; \
			addgroup -g ${ENV_GID} -S ${GROUP_NAME}; \
		fi

# Check if UID already exists
RUN cat /etc/passwd
RUN \
		if [ ${ENV_UID} -eq 0 ]; \
		then \
			adduser -S -h ${HOMEDIR_USER} -G ${GROUP_NAME} -s /bin/bash ${USER_NAME}; \
		elif grep -q -e "[^:][^:]*:[^:][^:]*:${ENV_UID}:.*$" /etc/passwd; \
		then \
			USER_NAME_ALREADY_EXISTS=$(grep  -e "[^:][^:]*:[^:][^:]*:${ENV_UID}:.*$" /etc/passwd | cut -f 1 -d':'); \
			echo "UID ${ENV_UID} already exists with user name ${USER_NAME_ALREADY_EXISTS}"; \
			usermod -g ${ENV_GID} -l ${USER_NAME} ${USER_NAME_ALREADY_EXISTS}; \
		else \
			echo "UID ${ENV_UID} does not exist"; \
			adduser -u ${ENV_UID} -S -h ${HOMEDIR_USER} -g ${ENV_GID} -G ${GROUP_NAME} -s /bin/bash ${USER_NAME}; \
		fi

		# && \
		# adduser  ${ENV_UID} -S -h ${HOMEDIR_USER} -G ${GROUP_NAME} -s /bin/bash ${USER_NAME}

# Remove package 'shadow'
RUN apk --no-cache del shadow

# Change passwords
RUN echo "root:rootDocker!" | chpasswd
RUN echo "${USER_NAME}:${PASSWORD_USER}" | chpasswd
# Allow ${USER_NAME} to execute sudo without password
RUN echo "${USER_NAME} ALL=(ALL:ALL) NOPASSWD: ALL" >> /etc/sudoers.d/local

##################################
# Create DOCKERIGNORED_DIR for all
##################################
RUN mkdir -p ${DOCKERIGNORED_DIR} \
		&& chmod 777 ${DOCKERIGNORED_DIR}

##################################
# Copy GIT deploy SSH key for user
##################################
RUN mkdir ${HOMEDIR_USER}/.ssh
RUN chmod 700 ${HOMEDIR_USER}/.ssh/
COPY ./DockerResources/id_rsa ${HOMEDIR_USER}/.ssh/
RUN chmod 600 ${HOMEDIR_USER}/.ssh/id_rsa
COPY ./DockerResources/id_rsa.pub ${HOMEDIR_USER}/.ssh/
RUN chmod 644 ${HOMEDIR_USER}/.ssh/id_rsa.pub
COPY ./DockerResources/known_hosts ${HOMEDIR_USER}/.ssh/
RUN chmod 644 ${HOMEDIR_USER}/.ssh/known_hosts
RUN chown -R ${USER_NAME}:${GROUP_NAME} ${HOMEDIR_USER}/.ssh/

##################################
# Set environment for user
##################################
# Change user and set workdir
USER ${USER_NAME}
WORKDIR ${HOMEDIR_USER}
RUN echo 'export PATH=/usr/local/bin:${PATH}' >> ${BASHRC_USER} \
		&& echo "alias ll='ls -l --color'" >> ${BASHRC_USER} \
		&&  echo ". ${BASHRC_USER}" >> ${HOMEDIR_USER}/.bashrc

# Copy SSH public key in authorized_keys
COPY ./DockerResources/id_rsa.pub ${HOMEDIR_USER}/.ssh/authorized_keys

# Test .dockerignore
RUN echo "Test" >> ${DOCKERIGNORED_DIR}/test

# Create DATADIR_USER
ENV DATADIR_USER=${HOMEDIR_USER}/data
RUN mkdir -p ${DATADIR_USER}

USER ${USER_NAME}
WORKDIR ${HOMEDIR_USER}
COPY ./DockerResources/entrypoint.sh /opt/docker/entrypoint.sh

EXPOSE 22
ENTRYPOINT ["/opt/docker/entrypoint.sh"]
# CMD ["/usr/sbin/sshd","-D"]
