
type ('a, 'b) result = Ok of 'a | Err of 'b

let (>>=) x f = match x with
  | Err v -> Err v
  | Ok v -> f v

let bind = (>>=)

let guard exp msg = if exp then Ok () else Err msg
