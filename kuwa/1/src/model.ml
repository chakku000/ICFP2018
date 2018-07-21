
type voxel = Full | Void

type 'a matrix = 'a array array array

type model = (int * voxel matrix)

let parse chan =
  let r = input_byte chan in

  let buf = Bytes.make 1 ' ' in
  let mtx =
    Array.init r (fun x ->
      Array.init r (fun y ->
        Array.init r (fun z ->
          let num = r*r*x + r*y + z in
          if num mod 8 = 0 then begin
            really_input chan buf 0 1;
          end;
          let byte = int_of_char (Bytes.get buf 0)in
          let ofs = num mod 8 in

          let b = (byte lsr ofs) land 1 in
          if b = 1 then Full else Void)))

  in (r, mtx)

let show_voxel = function
  | Full -> "#"
  | Void -> "."

let print (r, mtx) =
  Printf.printf "%d\n" r;
  for y = 0 to r-1 do
    for x = 0 to r-1 do
      for z = 0 to r-1 do
        Printf.printf "%c" @@
          match mtx.(x).(y).(z) with
          | Full -> '#' | Void -> '.'
      done;
      print_newline ()
    done;
    print_newline ()
  done;
