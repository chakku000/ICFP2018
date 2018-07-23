
open Command
open Trace
open Compress

module S = Scanf.Scanning

exception Bad_luck

let () =
  if Array.length Sys.argv < 2 then begin
    Printf.fprintf stderr "filename required\n";
    exit 1;
  end;
  let trace = open_in_bin Sys.argv.(1) |> parse_bin |> compress in
  match List.nth trace (List.length trace - 1) with
  | Halt -> trace |> print_bin
  | _ -> raise Bad_luck
