
open Result
open Interpret

let rec seq a b = if a > b then [] else a :: seq (a+1) b
let cmtx r = Array.init r (fun _ -> Array.make_matrix r r Model.Void)

let () =
  if Array.length Sys.argv < 3 then begin
    Printf.fprintf stderr "filename required\n";
    exit 1;
  end;
  Printf.printf "model: %s\n" Sys.argv.(1);
  Printf.printf "trace: %s\n%!" Sys.argv.(2);
  let (r,mtx) = Model.parse (open_in_bin Sys.argv.(1)) in
  let trace = Trace.parse_bin (open_in_bin Sys.argv.(2)) in
  let init_st : state = {
    enr = 0;
    hrm = Low;
    r = r;
    mtx = cmtx r;
    prv = -1;
    bots = [
      { bid = 1;
        pos = (0,0,0);
        seeds = seq 2 20; }
    ];
    trace = trace;
  } in

  run init_st
  |> begin function
    | Err msg -> Printf.printf "[error] %s\n" msg
    | Ok ({ enr; mtx=mtx' }) -> begin
          Printf.printf "energy: %d\n" enr;
          if mtx = mtx' then
            print_endline "success"
          else
            print_endline "fail"
    end;
  end

