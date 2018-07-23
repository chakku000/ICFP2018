
MODEL_OUT := model
DECODE_TRACE_OUT := decode_trace
ENCODE_TRACE_OUT := encode_trace
INTERPRET_OUT := interpret

COMPRESS_OUT := compress
ADJ_HRM1_OUT := opt1

OUT := $(MODEL_OUT) $(COMPRESS_OUT) $(DECODE_TRACE_OUT) $(ENCODE_TRACE_OUT) $(INTERPRET_OUT) $(ADJ_HRM1_OUT)

##################

OCAMLCC := ocamlopt.opt

SOURCE_DIR := base optimize
INCLUDES := $(addprefix -I ,$(SOURCE_DIR))
OCAML_OPTION := -O2 $(INCLUDES)

.PHONY: $(OUT)

all: interpret compress decode_trace encode_trace model adjust_harmonics1

interpret:
	$(OCAMLCC) $(OCAML_OPTION) base/result.ml base/model.ml base/command.ml base/trace.ml base/interpret.ml base/interpret_main.ml -o $(INTERPRET_OUT)

decode_trace:
	$(OCAMLCC) $(OCAML_OPTION) base/model.ml base/command.ml base/trace.ml base/decode_trace.ml -o $(DECODE_TRACE_OUT)

encode_trace:
	$(OCAMLCC) $(OCAML_OPTION) base/model.ml base/command.ml base/trace.ml base/encode_trace.ml -o $(ENCODE_TRACE_OUT)

model:
	$(OCAMLCC) $(OCAML_OPTION) base/model.ml base/model_main.ml -o $(MODEL_OUT)

compress:
	$(OCAMLCC) $(OCAML_OPTION) base/result.ml base/model.ml base/command.ml base/trace.ml optimize/compress.ml optimize/compress_main.ml -o $(COMPRESS_OUT)

adjust_harmonics1:
	$(OCAMLCC) $(OCAML_OPTION) base/result.ml base/model.ml base/command.ml base/trace.ml optimize/adjust_harmonics1.ml -o $(ADJ_HRM1_OUT)

clean:
	rm -f $(OUT) $(addsuffix /*.cm[aiox],$(SOURCE_DIR))
