
open Command
open Trace

module S = Scanf.Scanning

let () =
  if Array.length Sys.argv < 2 then begin
    Printf.fprintf stderr "filename required\n";
    exit 1;
  end;
  S.open_in Sys.argv.(1)
  |> parse_inst
  |> print_bin
