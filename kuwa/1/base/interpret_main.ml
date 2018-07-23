
open Result
open Interpret

let rec seq a b = if a > b then [] else a :: seq (a+1) b
let cmtx r = Array.init r (fun _ -> Array.make_matrix r r Model.Void)

let exists v a =
  Array.fold_left (fun b x -> b || v = x) false a

let () =
  if Array.length Sys.argv < 3 then begin
    Printf.fprintf stderr "filename required\n";
    exit 1;
  end;
  let argc = Array.length Sys.argv in
  
  let extras = if argc < 3 then [||] else Array.sub Sys.argv 3 (argc - 3) in
  let energy_flag = exists "energy" extras in
  let dump_flag = exists "dump" extras in
  let empty_flag = exists "empty" extras in

  let (r, mtx) = Model.parse (open_in_bin Sys.argv.(1)) in
  let trace = Trace.parse_bin (open_in_bin Sys.argv.(2)) in
  let init_st : state = {
    enr = 0;
    hrm = Low;
    r = r;
    mtx = if empty_flag then cmtx r else mtx;
    prv = -1;
    bots = [
      { bid = 1;
        pos = (0,0,0);
        seeds = seq 2 40; }
    ];
    trace = trace;
    gfill_queries = [];
    gvoid_queries = [];
  } in

  run init_st
  |> begin function
    | Err msg -> Printf.printf "[error] %s\n" msg
    | Ok ({ enr; mtx=mtx' }) -> begin
          if energy_flag then begin
            Printf.printf "%d\n" enr;
          end else if dump_flag then begin
            Model.print (r,mtx')
          end else begin
            Printf.printf "energy: %d\n" enr;
            Printf.printf "model: %s\n" Sys.argv.(1);
            Printf.printf "trace: %s\n%!" Sys.argv.(2);
            Printf.printf "# of commands: %d\n%!" (List.length trace);
            if mtx = mtx' then
              print_endline "success"
            else
              print_endline "fail"
          end
    end;
  end
