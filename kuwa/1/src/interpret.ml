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

let dim ((x1,y1,z1), (x2,y2,z2)) =
  let f a b = if a = b then 0 else 1 in
  f x1 x2 + f y1 y2 + f z1 z2

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
  gfill_queries : ((vec * vec) * int) list;
  gvoid_queries : ((vec * vec) * int) list;
}

let add_query ((r, c) as q) qs =
  List.filter (fun (r', _) -> r <> r') qs |> cons q

let rec run ({ bots; trace } as st) =
  let n = List.length bots in
  if n = 0 then begin
    if trace = [] then Ok st else Err "dead"
  end else begin

  let tr, trace' = take n trace, drop n trace in
  if List.length tr <> n then
    Err "lack of traces"
  else

  let rec loop tr ({ enr; hrm; r; bots; prv; trace; gfill_queries; gvoid_queries } as st) =
    match tr with
    (* end of traces *)
    | [] -> Ok st

    | (tr::trs) -> begin
      guard (List.length bots >= 1) "no active bot" >>= fun _ ->

      let ({ bid; pos; seeds } as bot) =
        find_opt (fun {bid=i} -> i > prv) bots
        |> function | None -> List.nth bots 0 | Some b -> b
      in
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

      | Void nd -> begin
        let (fx,fy,fz) = pos +: nd in
        (* TODO: validate pos, volatile coords *)
        let e = mtx.(fx).(fy).(fz) in
        mtx.(fx).(fy).(fz) <- Void;

        let enr' = enr + match e with Void -> -12 | Full -> 3 in
        loop trs { st with enr = enr' }
      end

      | Gfill (nd, fd) -> begin
        let (x1,y1,z1), (x2,y2,z2) = pos +: nd, pos +: nd +: fd in
        let ((sx,sy,sz), (tx,ty,tz) as region) =
          (min x1 x1, min y1 y2, min z1 z2), (max x1 x2, max y1 y2, max z1 z2) in
        let c =
          match find_opt (fun (r, _) -> r = region) gfill_queries with
          | None -> 1
          | Some (_, c) -> c+1
        in

        let q', fire =
          if c = (1 lsl dim region) then
            List.filter (fun (r, _) -> r <> region) gfill_queries, true
          else
            add_query (region, c) gfill_queries, false
        in

        let consumed =
          Array.init (tx-sx+1) (fun dx ->
            Array.init (ty-sy+1) (fun dy ->
              Array.init (tz-sz+1) (fun dz ->
                let x, y, z = sx+dx, sy+dy, sz+dz in
                let v = mtx.(x).(y).(z) in
                mtx.(x).(y).(z) <- Full;
                match v with Void -> 12 | Full -> 6)))
          |> Array.fold_left (Array.fold_left (Array.fold_left (+))) 0
        in

        loop trs { st with
          gfill_queries = q';
          enr = enr + consumed
        }
      end

      | Gvoid (nd, fd) -> begin
        let (x1,y1,z1), (x2,y2,z2) = pos +: nd, pos +: nd +: fd in
        let ((sx,sy,sz), (tx,ty,tz) as region) =
          (min x1 x1, min y1 y2, min z1 z2), (max x1 x2, max y1 y2, max z1 z2) in
        let c =
          match find_opt (fun (r, _) -> r = region) gvoid_queries with
          | None -> 1
          | Some (_, c) -> c+1
        in

        let q', fire =
          if c = (1 lsl dim region) then
            List.filter (fun (r, _) -> r <> region) gvoid_queries, true
          else
            add_query (region, c) gvoid_queries, false
        in

        let consumed =
          Array.init (tx-sx+1) (fun dx ->
            Array.init (ty-sy+1) (fun dy ->
              Array.init (tz-sz+1) (fun dz ->
                let x, y, z = sx+dx, sy+dy, sz+dz in
                let v = mtx.(x).(y).(z) in
                mtx.(x).(y).(z) <- Void;
                match v with Void -> 3 | Full -> -12)))
          |> Array.fold_left (Array.fold_left (Array.fold_left (+))) 0
        in

        loop trs { st with
          gvoid_queries = q';
          enr = enr + consumed
        }
      end
      | _ -> Err "not implemented"
    end
  in
  loop tr st >>= fun ({ gfill_queries; gvoid_queries } as st') ->
    if List.length gfill_queries <> 0 then
      Err "invalid GFill"
    else if List.length gvoid_queries <> 0 then
      Err "invalid GVoid"
    else
      run { st' with trace = trace' }
  end
