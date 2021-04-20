FROM debian:10 AS build

RUN apt update && apt install -y build-essential

WORKDIR /app

COPY mongoose.c mongoose.h cJSON.c cJSON.h ./
COPY main.c main.c
COPY Makefile Makefile

RUN make web
RUN chmod +x web

FROM debian:10-slim AS run

COPY --from=build /app/web .
ENTRYPOINT ["/web"]