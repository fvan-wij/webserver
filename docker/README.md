```
build container
docker build -t nginx_img .

run container
docker run -p 80:80 --name nginx nginx_img


delete all shit
docker system prune -a

list all built images
docker image ls
```
