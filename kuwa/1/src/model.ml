
type voxel = Full | Void

type 'a matrix = 'a array array array

type model = (int * voxel matrix)

let parse chan =
  let body = really_input_string chan (in_channel_length chan) in

  let r = int_of_char body.[0] in
  let mtx =
    Array.init r (fun x ->
      Array.init r (fun y ->
        Array.init r (fun z ->
          let num = r*r*x + r*y + z in
          let idx = 1 + num / 8 in
          let offset = (8 + num) - 8 * idx in

          let b = (int_of_char body.[idx] lsr (7-offset)) land 1 in
          if b = 1 then Full else Void)))
  in (r, mtx)

let show_voxel = function
  | Full -> "#"
  | Void -> "."

let print (r, mtx) =
  Printf.printf "%d\n" r;
  Array.iter (fun m ->
    Array.iter (fun row ->
      Array.iter (fun v -> print_string (show_voxel v)) row;
      print_newline ()) m;
    print_newline ()
  ) mtx;
