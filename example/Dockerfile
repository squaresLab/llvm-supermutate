FROM debian:bullseye-slim
WORKDIR /example
COPY foo.c .
COPY entrypoint.sh /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]
CMD ["/bin/bash"]
