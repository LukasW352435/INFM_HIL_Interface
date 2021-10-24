FROM gcc AS build

RUN apt-get update && apt-get install -y cmake libasio-dev libssl-dev
COPY ./ /tmp
WORKDIR /tmp
RUN mkdir build && cmake -S . -B build
RUN cmake --build build

FROM gcc AS run
COPY --from=build /tmp/build/REST_dummy_DuT /opt

EXPOSE 9090

ENTRYPOINT /opt/REST_dummy_DuT