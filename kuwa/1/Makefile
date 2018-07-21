
.PHONY: interpret model

MODEL_OUT := model
TRACE_OUT := trace
INTERPRET_OUT := interpret

OUT := $(MODEL_OUT) $(TRACE_OUT) $(INTERPRET_OUT)

all: interpret trace model

interpret:
	ocamlc -I src/ src/result.ml src/model.ml src/command.ml src/trace.ml src/interpret.ml src/interpret_main.ml -o $(INTERPRET_OUT)

trace:
	ocamlc -I src/ src/model.ml src/command.ml src/trace.ml src/trace_main.ml -o $(TRACE_OUT)

model:
	ocamlc -I src/ src/model.ml src/model_main.ml -o $(MODEL_OUT)

clean:
	rm -f $(OUT) src/*.cmi src/*.cmo
