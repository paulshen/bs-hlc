# bs-hlc

Hybrid Logical Clocks implemented in Reason/BuckleScript

## Installation

```sh
npm install --save bs-hlc
or
yarn add bs-hlc
```

In your `bsconfig.json`, include `"bs-hlc"` in the `bs-dependencies`.

## Usage

```reason
let myClock = ref(HLC.make(~node="myNodeId"));

type message = {
  content: string,
  clock: string, // serialized HLC
};

let sendMessage = (content) => {
  myClock := HLC.increment(myClock^);
  let message = {
    content,
    clock: HLC.toString(myClock^),
  };
};

let receiveMessage = remoteMessage => {
  myClock :=
    HLC.receive(
      myClock^,
      HLC.fromString(remoteMessage.clock) |> Belt.Option.getExn,
    );
};
```

## Credits

This is a BuckleScript port of @jlongster's [timestamp.js](https://github.com/jlongster/crdt-example-app/blob/3acd31069db65607bacd88a71c89fb43e53b6ec8/shared/timestamp.js) and @jaredly's [hlc.js](https://github.com/jaredly/hybrid-logical-clocks-example/blob/58af43df953b2a5eda806d4bfde354ad9406c188/src/hlc.js).
