#include "cJSON.h"
#include "mongoose.h"

static const char *s_listening_address = "tcp://0.0.0.0:8080";

static void listen_fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    int status = 0;

    if (mg_http_match_uri(hm, "/api/v1/execute")) {
      cJSON *cpu = cJSON_Parse(hm->body.ptr);
      if (cpu == NULL)
      {
          const char *error_ptr = cJSON_GetErrorPtr();
          if (error_ptr != NULL)
          {
              fprintf(stderr, "Parsing error before: %s\n", error_ptr);
          }
          status = -1;
          goto free_cpu;
      }

      const cJSON *state = cJSON_GetObjectItemCaseSensitive(cpu, "state");
      if (state == NULL) {
        fprintf(stderr, "Malformed JSON, missing state");
        status = -1;
        goto free_cpu;
      }

      cJSON *cycles = cJSON_GetObjectItemCaseSensitive(state, "cycles");
      if (cycles == NULL) {
        fprintf(stderr, "Malformed JSON, state missing cycles");
        status = -1;
        goto free_cpu;
      }

      cJSON_SetIntValue(cycles, cycles->valueint + 4);
free_cpu:
      if (status == 0) {
        mg_http_reply(c, 200, "", cJSON_PrintUnformatted(cpu));
      } else {
        mg_http_reply(c, 400, "", "");
      }
      cJSON_Delete(cpu);
    } else if (mg_http_match_uri(hm, "/status")) {
      mg_http_reply(c, 200, "", "Healthy");
    } else {
      mg_http_reply(c, 404, "", "");
    }

    mg_iobuf_delete(&c->recv, c->recv.len);   // And discard it
  }
}

int main(int argc, char *argv[]) {
  struct mg_mgr mgr;
  mg_mgr_init(&mgr);
  mg_http_listen(&mgr, s_listening_address, listen_fn, NULL);
  for (;;) mg_mgr_poll(&mgr, 1000);
  mg_mgr_free(&mgr);
  return 0;
}