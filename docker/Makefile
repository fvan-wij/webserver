NAME_IMG		:= nginx_img
NAME_CONTAINER	:= nginx

IS_IMG_BUILT	:= $(shell docker images | grep nginx_img)
LOCAL_VOLUME = $(CURDIR)/html

IS_CONTAINER_RUNNING := $(shell docker ps -a | grep $(NAME_CONTAINER) > /dev/null && docker container inspect -f '{{.State.Status}}' $(NAME_CONTAINER))

ifneq ($(IS_CONTAINER_RUNNING), running)
	RUN_CMD = docker run -p 80:80 -p 9999:9999 -p 8888:8888 -d -v $(LOCAL_VOLUME):/var/www/mysite/web --name $(NAME_CONTAINER) $(NAME_IMG)
else
	RUN_CMD = docker restart $(NAME_CONTAINER)
endif





.PHONY: all build run stop fclean

all: run
	docker ps


build:
	docker build -t $(NAME_IMG) .


run: build
	$(RUN_CMD)


bash:
	docker exec -it $(NAME_CONTAINER) bash


monitor:
	echo "tail the log file"


stop:
	docker stop $(NAME_CONTAINER) || true
	docker rm $(NAME_CONTAINER) || true


fclean: stop
	docker image rm $(NAME_IMG)

re: fclean all

