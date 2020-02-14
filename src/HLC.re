// Maximum physical clock drift allowed, in ms
let maxDrift = ref(60000.);
// Radix used to serialize counter to string
let counterRadix = 36;

type t = {
  timestamp: float,
  counter: int,
  node: string,
};
exception ClockDriftError;
exception CounterOverflowError;
exception DuplicateNodeError(string);

let setMaxDrift = value => {
  maxDrift := value;
};

let increment = (clock: t) => {
  let now = Js.Date.now();

  if (now > clock.timestamp) {
    {...clock, timestamp: now, counter: 0};
  } else {
    if (clock.timestamp -. now > maxDrift^) {
      raise(ClockDriftError);
    };
    let newCounter = clock.counter + 1;
    if (newCounter > 65535) {
      raise(CounterOverflowError);
    };
    {...clock, counter: newCounter};
  };
};

let receive = (localClock: t, remoteClock: t) => {
  let now = Js.Date.now();

  let remoteTimestamp = remoteClock.timestamp;

  if (remoteClock.node == localClock.node) {
    raise(DuplicateNodeError(remoteClock.node));
  };
  if (remoteTimestamp -. now > maxDrift^) {
    raise(ClockDriftError);
  };

  let (newTimestamp, newCounter) =
    if (now > localClock.timestamp && now > remoteClock.timestamp) {
      (now, 0);
    } else if (localClock.timestamp == remoteClock.timestamp) {
      (
        localClock.timestamp,
        Js.Math.max_int(localClock.counter, remoteClock.counter) + 1,
      );
    } else if (localClock.timestamp > remoteClock.timestamp) {
      (localClock.timestamp, localClock.counter + 1);
    } else {
      (remoteClock.timestamp, remoteClock.counter + 1);
    };

  if (newTimestamp -. now > maxDrift^) {
    raise(ClockDriftError);
  };
  if (newCounter > 65535) {
    raise(CounterOverflowError);
  };

  {...localClock, timestamp: newTimestamp, counter: newCounter};
};

[@bs.send] external padStart: (string, int, string) => string = "padStart";

let toString = (clock: t) => {
  Js.Array.joinWith(
    "-",
    [|
      Js.Float.toString(clock.timestamp)->padStart(15, "0"),
      Js.Int.toStringWithRadix(clock.counter, ~radix=counterRadix)
      ->padStart(4, "0"),
      clock.node,
    |],
  );
};

[@bs.val]
external parseIntWithRadix: (string, ~radix: int) => int = "parseInt";

let fromString = (input: string) => {
  let parts = input |> Js.String.split("-");
  if (Js.Array.length(parts) != 3) {
    None;
  } else {
    let timestamp = Js.Float.fromString(parts[0]);
    let counter = parseIntWithRadix(parts[1], ~radix=counterRadix);
    let node = parts[2];
    if (!Js.Float.isNaN(timestamp) && !Js.Float.isNaN(float_of_int(counter))) {
      Some({timestamp, counter, node});
    } else {
      None;
    };
  };
};

let timestampToString = (timestamp: float) => {
  Js.Float.toString(timestamp)->padStart(15, "0") ++ "-0000-0000";
};