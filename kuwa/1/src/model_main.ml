
open Model

let exists v a =
  Array.fold_left (fun b x -> b || v = x) false a

let () =
  if Array.length Sys.argv < 2 then begin
    Printf.fprintf stderr "filename required\n";
    exit 1;
  end;

  let argc = Array.length Sys.argv in
  let extras = if argc < 2 then [||] else Array.sub Sys.argv 2 (argc - 2) in
  let empty_flag = exists "empty" extras in

  let (r, mtx) = open_in_bin Sys.argv.(1) |> parse in

  if empty_flag then begin
    for i = 0 to r*r*r-1 do
      mtx.(i/r/r).(i/r mod r).(i mod r) <- Void
    done
  end;

  print (r, mtx)

