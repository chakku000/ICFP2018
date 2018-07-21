
open Command
open Trace
open Compress

module S = Scanf.Scanning

let () =
  if Array.length Sys.argv < 2 then begin
    Printf.fprintf stderr "filename required\n";
    exit 1;
  end;
  open_in_bin Sys.argv.(1)
  |> parse_bin
  |> compress
  |> print_bin

