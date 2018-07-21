
open Command

exception Invalid_trace
(* short coordinate difference *)
let decode_sld a i =
  match a with
  | 0b01 -> (i-5, 0, 0)
  | 0b10 -> (0, i-5, 0)
  | 0b11 -> (0, 0, i-5)
  | _ -> raise Invalid_trace

(* long coordinate difference *)
let decode_lld a i =
  match a with
  | 0b01 -> (i-15, 0, 0)
  | 0b10 -> (0, i-15, 0)
  | 0b11 -> (0, 0, i-15)
  | _ -> raise Invalid_trace

(* near coordinate difference *)
let decode_ncd nd =
  nd/9 - 1,
  (nd mod 9) / 3 - 1,
  (nd mod 3) - 1

(* parse: parse .nbt file *)
let parse chan = begin
  let len = in_channel_length chan in
  let body = really_input_string chan len in

  let rec loop idx =
    if idx >= len then []
    else begin
      let cmd, idx' = begin
        let b = int_of_char body.[idx] in
        match b with
        | 0b11111111 -> Halt, idx+1
        | 0b11111110 -> Wait, idx+1
        | 0b11111101 -> Flip, idx+1
        | _ -> begin
          match b land 0b1111, b land 0b111 with
          | 0b0100, _ -> begin (* SMove *)
            let a = (b lsr 4) land 0b11 in
            let i = int_of_char body.[idx+1] land 0b11111 in
            Smove (decode_lld a i), idx+2
          end
          | 0b1100, _ -> begin (* LMove *)
            let a2 = (b lsr 6) land 0b11 in
            let a1 = (b lsr 4) land 0b11 in
            let b' = int_of_char body.[idx+1] in
            let i2, i1 = (b' lsr 4), b' land 0b1111 in
            Lmove (decode_lld a1 i1, decode_lld i2 a2), idx+2
          end
          | _, 0b111 -> begin (* FusionP *)
            let nd = (b lsr 3) land 0b11111 in
            Fusion1 (decode_ncd nd), idx+1
          end
          | _, 0b110 -> begin (* FusionS *)
            let nd = (b lsr 3) land 0b11111 in
            Fusion2 (decode_ncd nd), idx+1
          end
          | _, 0b101 -> begin (* Fission *)
            let nd = (b lsr 3) land 0b11111 in
            let b' = int_of_char body.[idx+1] in
            Fission (decode_ncd nd, b'), idx+2
          end
          | _, 0b011 -> begin (* Fill *)
            let nd = (b lsr 3) land 0b11111 in
            Fill (decode_ncd nd), idx+1
          end
          | _ -> raise Invalid_trace
        end
      end in
      cmd :: loop idx'
    end
  in loop 0
end

let print =
  List.iter (fun cmd ->
    print_endline (show_command cmd))
