
MODEL_OUT := model
DECODE_TRACE_OUT := decode_trace
ENCODE_TRACE_OUT := encode_trace
INTERPRET_OUT := interpret

COMPRESS_OUT := compress

OUT := $(MODEL_OUT) $(COMPRESS_OUT) $(DECODE_TRACE_OUT) $(ENCODE_TRACE_OUT) $(INTERPRET_OUT)

##################

OCAMLCC := ocamlopt.opt

SOURCE_DIR := base optimize
INCLUDES := $(addprefix -I ,$(SOURCE_DIR))

.PHONY: $(OUT)

all: interpret compress decode_trace encode_trace model

interpret:
	$(OCAMLCC) $(INCLUDES) base/result.ml base/model.ml base/command.ml base/trace.ml base/interpret.ml base/interpret_main.ml -o $(INTERPRET_OUT)

decode_trace:
	$(OCAMLCC) $(INCLUDES) base/model.ml base/command.ml base/trace.ml base/decode_trace.ml -o $(DECODE_TRACE_OUT)

encode_trace:
	$(OCAMLCC) $(INCLUDES) base/model.ml base/command.ml base/trace.ml base/encode_trace.ml -o $(ENCODE_TRACE_OUT)

model:
	$(OCAMLCC) $(INCLUDES) base/model.ml base/model_main.ml -o $(MODEL_OUT)

compress:
	$(OCAMLCC) $(INCLUDES) base/result.ml base/model.ml base/command.ml base/trace.ml optimize/compress.ml optimize/compress_main.ml -o $(COMPRESS_OUT)

clean:
	rm -f $(OUT) $(addsuffix /*.cm[aiox],$(SOURCE_DIR))
