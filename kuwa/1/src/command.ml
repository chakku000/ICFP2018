
type vec = (int * int * int)

let (+:) (x1,y1,z1) (x2,y2,z2) = (x1+x2, y1+y2, z1+z2)

let mlen (x1,y1,z1) = abs x1 + abs y1 + abs z1

let clen (x1,y1,z1) = max (abs x1) (max (abs y1) (abs z1))

type command =
  | Halt
  | Wait
  | Flip
  | Smove of vec
  | Lmove of (vec * vec)
  | Fission of (vec * int)
  | Fill of vec
  | Fusion1 of vec
  | Fusion2 of vec
  (* new commands *)
  | Void of vec
  | Gfill of (vec * vec)
  | Gvoid of (vec * vec)

open Printf

let show_vec (x,y,z) =
  sprintf "%d %d %d" x y z

let show_command = function
  | Halt -> "Halt"
  | Wait -> "Wait"
  | Flip -> "Flip"
  | Smove d -> sprintf "SMove %s" (show_vec d)
  | Lmove (d1, d2) ->
      sprintf "Lmove %s %s" (show_vec d1) (show_vec d2)
  | Fission (d, m) ->
      sprintf "Fission %s %d" (show_vec d) m
  | Fill d ->
      sprintf "Fill %s" (show_vec d)
  | Fusion1 d ->
      sprintf "FusionP %s" (show_vec d)
  | Fusion2 d ->
      sprintf "FusionS %s" (show_vec d)
  | Void d ->
      sprintf "Void %s" (show_vec d)
  | Gfill (nd, fd) ->
      sprintf "GFill %s %s" (show_vec nd) (show_vec fd)
  | Gvoid (nd, fd) ->
      sprintf "GVoid %s %s" (show_vec nd) (show_vec fd)


