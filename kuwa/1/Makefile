
MODEL_OUT := model
COMPRESS_OUT := compress
DECODE_TRACE_OUT := decode_trace
ENCODE_TRACE_OUT := encode_trace
INTERPRET_OUT := interpret

OUT := $(MODEL_OUT) $(COMPRESS_OUT) $(DECODE_TRACE_OUT) $(ENCODE_TRACE_OUT) $(INTERPRET_OUT)

OCAMLCC := ocamlc.opt

.PHONY: $(OUT)

all: interpret compress decode_trace encode_trace model

interpret:
	$(OCAMLCC) -I src/ src/result.ml src/model.ml src/command.ml src/trace.ml src/interpret.ml src/interpret_main.ml -o $(INTERPRET_OUT)

compress:
	$(OCAMLCC) -I src/ src/result.ml src/model.ml src/command.ml src/trace.ml src/compress.ml src/compress_main.ml -o $(COMPRESS_OUT)

decode_trace:
	$(OCAMLCC) -I src/ src/model.ml src/command.ml src/trace.ml src/decode_trace.ml -o $(DECODE_TRACE_OUT)

encode_trace:
	$(OCAMLCC) -I src/ src/model.ml src/command.ml src/trace.ml src/encode_trace.ml -o $(ENCODE_TRACE_OUT)

model:
	$(OCAMLCC) -I src/ src/model.ml src/model_main.ml -o $(MODEL_OUT)

clean:
	rm -f $(OUT) src/*.cmi src/*.cmo src/*.cmx
