#!/bin/sh
podman run --rm -i -p 8888:80 -v .:/usr/share/nginx/html:ro nginx:alpine
