include Makefile.env

NS ?= ns_unknown
IMAGE_NAME ?= image_name_unknown
VERSION ?= version_unknown
CONTAINER_NAME ?= container_name_unknown
CONTAINER_INSTANCE ?= container_instance_unknown

# Conditional variables
ENV_UID ?= 0
ENV_GID ?= 0

.PHONY: build push shell run run_xlocal run_ssh up down ps rm release

build: $(DOCKERFILE)
	docker build --build-arg ENV_UID="$(ENV_UID)" --build-arg ENV_GID="$(ENV_GID)" -t $(NS)/$(IMAGE_NAME):$(VERSION) -f $(DOCKERFILE) .

up:
	# Start docker container as SSH service
	@echo "Starting docker container as SSH service."
	docker run --rm -d --name $(CONTAINER_NAME)-$(CONTAINER_INSTANCE) $(PORTS) $(VOLUMES) $(ENV) $(NS)/$(IMAGE_NAME):$(VERSION) 'sudo /usr/sbin/sshd -D'

down:
	# Stop docker container as SSH service
	docker stop $(CONTAINER_NAME)-$(CONTAINER_INSTANCE)

ps:
	docker ps -f name=$(CONTAINER_NAME)-$(CONTAINER_INSTANCE)

run_ssh:
	@echo "WARNING: make run_ssh needs docker container SSH service."
	ssh-keygen -R [localhost]:$(SSH_CONTAINER_PORT)
	chmod 600 ./DockerResources/id_rsa
	# X11 port forwarding by ssh -Y option
	ssh -i ./DockerResources/id_rsa -oStrictHostKeyChecking=no -Y -p $(SSH_CONTAINER_PORT) teseo@localhost 'export `dbus-launch`; dbus-launch gimp; kill -TERM $$DBUS_SESSION_BUS_PID' || (echo ""; echo "WARNING: Did you run 'make start' before run_ssh?"; echo "";)

run_xlocal:
	@echo "WARNING: make run_xlocal works only on Linux."
	docker run --rm -it --privileged --name $(CONTAINER_NAME)-$(CONTAINER_INSTANCE) $(PORTS) $(VOLUMES) $(ENV) $(ENV_X_LOCALSOCKET) $(NS)/$(IMAGE_NAME):$(VERSION)

shell:
	docker run --rm --name $(CONTAINER_NAME)-$(CONTAINER_INSTANCE) -i -t $(PORTS) $(VOLUMES) $(ENV) $(NS)/$(IMAGE_NAME):$(VERSION) /bin/bash

rm:
	docker rm $(CONTAINER_NAME)-$(CONTAINER_INSTANCE)

release: build
	make push -e VERSION=$(VERSION)

default: build

push:
	docker push $(NS)/$(IMAGE_NAME):$(VERSION)

