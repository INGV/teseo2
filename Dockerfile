# Docker for GIMP Development Environment based on alpine
FROM alpine

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
		glade3 \
		gimp-dev

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
ENV GTKDATABOX_VERSION=0.9.3.0
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
		&&  ./configure && make && sudo make install

##################################
# Create user and add to sudo users
##################################
RUN addgroup -S ${GROUP_NAME} \
		&& adduser -S -h ${HOMEDIR_USER} -G ${GROUP_NAME} -s /bin/bash ${USER_NAME}
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

# Test .dockerignore
RUN echo "Test" >> ${DOCKERIGNORED_DIR}/test

# Create DATADIR_USER
ENV DATADIR_USER=${HOMEDIR_USER}/data
RUN mkdir -p ${DATADIR_USER}

USER ${USER_NAME}
WORKDIR ${HOMEDIR_USER}
COPY ./DockerResources/entrypoint.sh /opt/docker/entrypoint.sh
ENTRYPOINT ["/opt/docker/entrypoint.sh"]
