type t = {
  timestamp: float,
  counter: int,
  node: string,
};

exception ClockDriftError;
exception OverflowError;
exception DuplicateNodeError(string);

let setMaxDrift: float => unit;

let increment: t => t;
let receive: (t, t) => t;

let toString: t => string;
let fromString: string => option(t);

let timestampToString: float => string;