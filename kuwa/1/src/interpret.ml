open Model
open Command
open Result

(* utility *)

let rec take n = function
  | x::xs when n > 0 -> x :: take (n-1) xs
  | _ -> []

let rec drop n = function
  | x::xs when n > 0 -> drop (n-1) xs
  | xs -> xs

let cons x xs = x::xs

let rec find_opt f = function
  | [] -> None
  | x::xs -> if f x then Some x else find_opt f xs

(* END utility *)

type id = int
type harmonics = Low | High

type bot = {
  bid : id;
  pos : vec;
  seeds : id list;
}

let cmp_bot {bid=i1} {bid=i2} = i1-i2
let add_bot ({bid=i} as b) bs =
  List.filter (fun {bid=j} -> i <> j) bs |> cons b
  |> List.sort cmp_bot

type state = {
  enr : int;
  hrm : harmonics;
  r : int;
  mtx : voxel matrix;
  prv : int; (* the ID of last active nanobot *)
  bots : bot list;
  trace : command list;
}

let rec run ({ bots; trace } as st) =
  let n = List.length bots in
  if n = 0 then begin
    if trace = [] then Ok st else Err "dead"
  end else begin

  let tr, trace' = take n trace, drop n trace in
  if List.length tr <> n then
    Err "lack of traces"
  else

  let rec loop tr ({ enr; hrm; r; bots; prv; trace } as st) =
    match tr with
    (* end of traces *)
    | [] -> Ok st

    | (tr::trs) -> begin
      guard (List.length bots >= 1) "no active bot" >>= fun _ ->

      let ({ bid; pos; seeds } as bot) =
        find_opt (fun {bid=i} -> i > prv) bots
        |> function | None -> List.nth bots 0 | Some b -> b
      in
      Printf.printf "-- %s at %s\n" (show_command tr) (show_vec pos);
      let add_cost =
        let { bid=b' } = List.nth bots 0 in
        if bid = b' then
          (* cardinal energy *)
          20 * List.length bots +
          (* harmonics energy *)
          match hrm with | High -> 30 * r * r * r | _ -> 3 * r * r * r
        else 0
      in

      let ({ enr; hrm; r; mtx; prv; bots; trace } as st) =
        { st with prv = bid; trace = trs; enr = enr + add_cost } in
      match tr with

      (* Halt *)
      | Halt -> begin
        guard (hrm = Low) "[Halt] harmonics must be Low" >>= fun _ ->
        guard (List.length bots = 1) "[Halt] # of bots must be 1" >>= fun _ ->
        let bx,by,bz = pos in
        guard (bx = 0 && by = 0 && bz = 0) "[Halt] c_0 must be (0,0,0)" >>= fun _ ->
        loop trs { st with bots = []; }
      end

      (* Wait *)
      | Wait -> loop trs st

      (* Flip *)
      | Flip -> begin
        let hrm' = match hrm with High -> Low | Low -> High in
        loop trs { st with hrm = hrm' }
      end

      (* SMove *)
      | Smove d -> begin
        let bot' = { bot with pos = pos +: d } in
        let bots' = add_bot bot' bots in
        (* TODO: validate pos, volatile coords *)
        let enr' = enr + 2 * mlen d in
        loop trs { st with bots = bots'; enr = enr' }
      end

      (* LMove *)
      | Lmove (d1,d2) -> begin
        let bot' = { bot with pos = pos +: d1 +: d2 } in
        let bots' = add_bot bot' bots in
        (* TODO: validate pos, volatile coords *)
        let enr' = enr + 2 * (mlen d1 + 2 + mlen d2) in
        loop trs { st with bots = bots'; enr = enr' }
      end

      (* Fill *)
      | Fill d -> begin
        let (fx,fy,fz) = pos +: d in
        (* TODO: validate pos, volatile coords *)
        let e = mtx.(fx).(fy).(fz) in
        mtx.(fx).(fy).(fz) <- Full;

        let enr' = enr + match e with Void -> 12 | Full -> 6 in
        loop trs { st with enr = enr' }
      end

      (* Fission *)
      | Fission (d, m) -> begin
        guard (List.length seeds >= m+1) "[Fission] lack of seeds" >>= fun _ ->
        let child_bid = List.nth seeds 0 in
        let child = {
          bid = child_bid;
          pos = pos +: d;
          seeds = seeds |> drop 1 |> take m;
        } in
        (* TODO: validate pos, volatile coords *)
        let bot' = { bot with seeds = drop (m+1) seeds; } in
        (* TODO: validate pos, volatile coords *)
        let bots' =
          List.filter (fun {bid=i} -> i <> bid && i <> child_bid) bots
          |> cons child |> cons bot' |> List.sort cmp_bot
        in

        let enr' = enr + 24 in
        loop trs { st with bots = bots'; enr = enr' }
      end
      | _ -> Err "not implemented"
    end
  in
  loop tr st >>= fun st' -> run { st' with trace = trace' }
  end
