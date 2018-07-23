
open Command

let compress trace =
  let rec loop (x,y,z) = function
    | [] -> begin
      if abs x + abs y + abs z > 0 then
        Smove (x,y,z) :: []
      else
        []
    end
    | t::ts -> begin
      match t with
      | Smove (i,j,k) -> begin
        match (x,y,z), (i,j,k) with
        | (x,0,0), (x',0,0) when abs (x+x') <= 15 -> loop (x+x',0,0) ts
        | (0,y,0), (0,y',0) when abs (y+y') <= 15 -> loop (0,y+y',0) ts
        | (0,0,z), (0,0,z') when abs (z+z') <= 15 -> loop (0,0,z+z') ts
        | _, _ -> Smove (x,y,z) :: loop (i,j,k) ts
      end
      | _ -> begin
        if abs x + abs y + abs z > 0 then
          Smove (x,y,z) :: t :: loop (0,0,0) ts
        else
          t :: loop (0,0,0) ts
      end
    end

  in loop (0,0,0) trace
