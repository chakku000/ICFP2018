
open Command
exception Invalid_trace of string
exception Invalid_sld
exception Invalid_lld

(*
 * decode
 *)

(* short coordinate difference *)
let decode_sld a i =
  match a with
  | 0b01 -> (i-5, 0, 0)
  | 0b10 -> (0, i-5, 0)
  | 0b11 -> (0, 0, i-5)
  | _ -> raise Invalid_sld

(* long coordinate difference *)
let decode_lld a i =
  match a with
  | 0b01 -> (i-15, 0, 0)
  | 0b10 -> (0, i-15, 0)
  | 0b11 -> (0, 0, i-15)
  | _ -> raise Invalid_lld

(* far coordinate difference *)
let decode_fd x y z = (x-30, y-30, z-30)

(* near coordinate difference *)
let decode_ncd nd =
  nd/9 - 1,
  (nd mod 9) / 3 - 1,
  (nd mod 3) - 1

(* parse: parse .nbt file *)
let parse_bin chan = begin
  let read_byte =
    let buf = Bytes.make 1 '0' in
    fun () ->
      try really_input chan buf 0 1; Some (Bytes.get buf 0 |> int_of_char)
      with _ -> None
  in
  let (>>=) x f =
    match x with
    | None -> None
    | Some v -> f v
  in

  let rec loop () =
    match read_byte () with
    | None -> []
    | Some b -> begin
      let cmd = match b with
      | 0b11111111 -> Halt
      | 0b11111110 -> Wait
      | 0b11111101 -> Flip
      | _ -> begin
        match b land 0b1111, b land 0b111 with
        | 0b0100, _ -> begin (* SMove *)
          let a = (b lsr 4) land 0b11 in
          match read_byte () with
          | None -> raise (Invalid_trace "SMove")
          | Some b' -> let i = b' land 0b11111 in Smove (decode_lld a i)
        end
        | 0b1100, _ -> begin (* LMove *)
          let a2 = (b lsr 6) land 0b11 in
          let a1 = (b lsr 4) land 0b11 in
          match read_byte () with
          | None -> raise (Invalid_trace "LMove")
          | Some b' ->
             let i2, i1 = b' lsr 4, b' land 0b11111 in
             Lmove (decode_sld a1 i1, decode_sld a2 i2)
        end
        | _, 0b111 -> begin (* FusionP *)
          let nd = (b lsr 3) land 0b11111 in
          Fusion1 (decode_ncd nd)
        end
        | _, 0b110 -> begin (* FusionS *)
          let nd = (b lsr 3) land 0b11111 in
          Fusion2 (decode_ncd nd)
        end
        | _, 0b101 -> begin (* Fission *)
          let nd = (b lsr 3) land 0b11111 in
          match read_byte () with
          | None -> raise (Invalid_trace "Fission")
          | Some b' -> Fission (decode_ncd nd, b')
        end
        | _, 0b011 -> begin (* Fill *)
          let nd = (b lsr 3) land 0b11111 in
          Fill (decode_ncd nd)
        end
        | _, 0b010 -> begin (* Void *)
          let nd = (b lsr 3) land 0b11111 in
          Void (decode_ncd nd)
        end
        | _, 0b001 -> begin (* Gfill *)
          let nd = (b lsr 3) land 0b11111 in
          begin
            read_byte () >>= fun b1 ->
            read_byte () >>= fun b2 ->
            read_byte () >>= fun b3 ->
              Some (Gfill (decode_ncd nd, decode_fd b1 b2 b3))
          end |> function
            | Some c -> c
            | None -> raise (Invalid_trace "Gfill")
        end
        | _, 0b000 -> begin (* Gvoid *)
          let nd = (b lsr 3) land 0b11111 in
          begin
            read_byte () >>= fun b1 ->
            read_byte () >>= fun b2 ->
            read_byte () >>= fun b3 ->
              Some (Gvoid (decode_ncd nd, decode_fd b1 b2 b3))
          end |> function
            | Some c -> c
            | None -> raise (Invalid_trace "Gfill")
        end
        | _ -> raise (Invalid_trace "Unknown")
      end
      in
      cmd :: loop ()
    end
  in loop ()
end

let print =
  List.iter (fun cmd ->
    print_endline (show_command cmd))

(*
 * encode
 *)

exception End_of_inst
exception Invalid_command

let parse_inst chan =
  let scan fmt = Scanf.bscanf chan fmt in
  let f s = 
    s |> function
    | "Halt" -> Halt
    | "Wait" -> Wait
    | "Flip" -> Flip
    | "SMove" -> begin
      scan " %d %d %d" (fun x y z -> Smove (x,y,z))
    end
    | "LMove" -> begin
      scan " %d %d %d %d %d %d" (fun x1 y1 z1 x2 y2 z2 -> Lmove ((x1,y1,z1), (x2,y2,z2)))
    end
    | "Fission" -> begin
      scan " %d %d %d %d" (fun x y z m -> Fission ((x,y,z), m))
    end
    | "Fill" -> begin
      scan " %d %d %d" (fun x y z -> Fill (x,y,z))
      end
    | "FusionP" -> begin
      scan " %d %d %d" (fun x y z -> Fusion1 (x,y,z))
    end
    | "FusionS" -> begin
      scan " %d %d %d" (fun x y z -> Fusion2 (x,y,z))
    end
    | "Void" -> begin
      scan " %d %d %d" (fun x y z -> Void (x,y,z))
    end
    | "GFill" -> begin
      scan " %d %d %d %d %d %d" (fun x1 y1 z1 x2 y2 z2 -> Gfill ((x1,y1,z1), (x2,y2,z2)))
    end
    | "GVoid" -> begin
      scan " %d %d %d %d %d %d" (fun x1 y1 z1 x2 y2 z2 -> Gvoid ((x1,y1,z1), (x2,y2,z2)))
    end
    | _ -> raise Invalid_command
  in
  let rec loop () =
    try
      let cmd = scan " %s" (fun s -> 
        if s = "" then raise End_of_inst;
        f s) in
      cmd :: loop ()
    with
      End_of_inst | Scanf.Scan_failure _ -> []
  in loop ()

let encode_sld = function
  | x,0,0 -> (0b01, x+5)
  | 0,y,0 -> (0b10, y+5)
  | 0,0,z -> (0b11, z+5)
  | _ -> raise Invalid_sld

let encode_lld = function
  | x,0,0 -> (0b01, x+15)
  | 0,y,0 -> (0b10, y+15)
  | 0,0,z -> (0b11, z+15)
  | _ -> raise Invalid_lld

let encode_nd (x,y,z) = (x+1) * 9 + (y+1) * 3 + (z+1)

let encode_fd (x,y,z) = (x+30, y+30,z+30)

let encode_cmd cmd =
  match cmd with
  | Halt -> [0b11111111]
  | Wait -> [0b11111110]
  | Flip -> [0b11111101]
  | Smove lld -> begin
    let a, i = encode_lld lld in
    [(a lsl 4) + 0b0100; i]
  end
  | Lmove (sld1,sld2) -> begin
    let a1, i1 = encode_sld sld1 in
    let a2, i2 = encode_sld sld2 in
    [(a2 lsl 6) + (a1 lsl 4) + 0b1100; (i2 lsl 4) + i1]
  end
  | Fission (nd, m) -> begin
    [(encode_nd nd lsl 3) + 0b101; m]
  end
  | Fill nd -> begin
    [(encode_nd nd lsl 3) + 0b011]
  end
  | Fusion1 nd -> begin
    [(encode_nd nd lsl 3) + 0b111]
  end
  | Fusion2 nd -> begin
    [(encode_nd nd lsl 3) + 0b110]
  end
  | Void nd -> begin
    [(encode_nd nd lsl 3) + 0b010]
  end
  | Gfill (nd, fd) -> begin
    let (ex,ey,ez) = encode_fd fd in
    [(encode_nd nd lsl 3) + 0b001; ex; ey; ez]
  end
  | Gvoid (nd, fd) -> begin
    let (ex,ey,ez) = encode_fd fd in
    [(encode_nd nd lsl 3) + 0b000; ex; ey; ez]
  end

let print_bin trace =
  List.iter (fun t -> List.iter (output_byte stdout) (encode_cmd t)) trace
