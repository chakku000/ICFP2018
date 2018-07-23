
open Model
open Command
open Interpret

let flip = function Low -> High | High -> Low

let detect_harmonics_fa (r,mtx) =
  let res = Array.init r (fun _ -> Array.make_matrix r r High) in

  for x = 1 to r-1 do
    for z = 1 to r-1 do
      res.(x).(0).(z) <- 
        match mtx.(x).(0).(z) with Full -> Low | _ -> High
    done
  done;

  for y = 1 to r-2 do
    let visited = Array.make_matrix r r false in
    let rec fill_low_cc x z =
      visited.(x).(z) <- true;
      res.(x).(y).(z) <- Low;
      [(x+1,z); (x,z+1); (x-1,z); (x,z-1)] |> List.iter (fun (x',z') ->
        match mtx.(x').(y).(z'), mtx.(x').(y-1).(z'), visited.(x').(z') with
        | Full, Full, false -> fill_low_cc x' z'
        | _ -> ())
    in

    for x = 1 to r-2 do
      for z = 1 to r-2 do
        if mtx.(x).(y).(z) = Full && res.(x).(y-1).(z) = Low && not visited.(x).(z) then
          fill_low_cc x z;
      done
    done;
  done;
  res

let rec for_fold a b f v = if a >= b then v else for_fold (a+1) b f (f v a)

let detect_harmonics_y (r,mtx) hm_fa =
  Array.init r (fun y ->
    if y = 0 then Low
    else
      for_fold 0 r (fun b x ->
        b && for_fold 0 r (fun b z ->
          b && (mtx.(x).(y).(z) = Model.Void || hm_fa.(x).(y-1).(z) = Low)) true) true
      |> fun b -> if b then Low else High)
  
let optimize_flips mdl trace = begin
  let hm = detect_harmonics_fa mdl |> detect_harmonics_y mdl in

  let rec loop y h rs ts =
    match ts with
    | [] -> List.rev rs
    | Halt::_ -> begin
      match h with Low -> List.rev (Halt::rs) | High -> (Flip::Halt::rs)
    end
    | t::ts -> begin
      match t with
      | Flip -> loop y h rs ts
      | Smove (_,dy,_) -> begin
        let y' = y+dy in
        if (y' = 0 || hm.(y'-1) = Low) && h = High then
          loop y' Low (t::Flip::rs) ts
        else
          loop (y+dy) h (t::rs) ts
      end
      | Lmove ((_,dy1,_), (_,dy2,_)) -> begin
        loop (y+dy1+dy2) h (t::rs) ts
      end
      | Fill (_,dy,_) | Void (_,dy,_)  -> begin
        let y' = y+dy in
        match hm.(y') with
        | High when h = Low -> loop y High (t::Flip::rs) ts
        | _                 -> loop y h (t::rs) ts
      end
      | Gfill ((_,dy1,_), (_,dy2,_)) | Gvoid ((_,dy1,_), (_,dy2,_)) -> begin
        let y0,y1  = min (y+dy1) (y+dy1+dy2), max (y+dy1) (y+dy1+dy2) in
        for_fold y0 (y1+1) (fun b y ->
          match b with High -> High | _ -> hm.(y)) Low
        |> function
          | High when h = Low -> loop y High (t::Flip::rs) ts
          | _                 -> loop y h (t::rs) ts
      end
      | _ -> loop y h (t::rs) ts
    end
  in

  loop 0 Low [] trace
end

let () =
  if Array.length Sys.argv < 3 then begin
    Printf.fprintf stderr "Usage: %s mdl_file nbt_file\n" Sys.argv.(0);
    exit 1;
  end;

  let mdl = Model.parse (open_in_bin Sys.argv.(1)) in

  let nbt = Trace.parse_bin (open_in_bin Sys.argv.(2)) in
  let nbt' = optimize_flips mdl nbt in

  Trace.print_bin nbt'
