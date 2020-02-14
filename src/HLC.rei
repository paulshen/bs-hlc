type t = {
  timestamp: float,
  counter: int,
  node: string,
};

exception ClockDriftError;
exception CounterOverflowError;
exception DuplicateNodeError(string);

let increment: t => t;
let receive: (t, t) => t;

let toString: t => string;
let fromString: string => option(t);

let timestampToString: float => string;

module Config: {let setMaxDrift: float => unit;};