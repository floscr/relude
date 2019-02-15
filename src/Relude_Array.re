let length: array('a) => int = Belt.Array.length;

let isEmpty: array('a) => bool = arr => length(arr) == 0;

let isNotEmpty: array('a) => bool = arr => length(arr) > 0;

let empty: array('a) = [||];

let pure: 'a => array('a) = x => [|x|];

let one: 'a => array('a) = pure;

let repeat: (int, 'a) => array('a) = (i, x) => Belt.Array.make(i, x);

let makeWithIndex: (int, int => 'a) => array('a) = Belt.Array.makeBy;

let concat: (array('a), array('a)) => array('a) = Belt.Array.concat;

let cons: ('a, array('a)) => array('a) = (x, xs) => concat([|x|], xs);

let uncons: array('a) => option(('a, array('a))) =
  xs =>
    switch (xs) {
    | [||] => None
    | _ => Some((Belt.Array.getExn(xs, 0), Belt.Array.sliceToEnd(xs, 1)))
    };

let prepend: ('a, array('a)) => array('a) = cons;

let append: ('a, array('a)) => array('a) =
  (item, array) => concat(array, [|item|]);

let foldLeft: (('b, 'a) => 'b, 'b, array('a)) => 'b = BsAbstract.Array.Foldable.fold_left;

let foldRight: (('a, 'b) => 'b, 'b, array('a)) => 'b = BsAbstract.Array.Foldable.fold_right;

let scanLeft: (('b, 'a) => 'b, 'b, array('a)) => array('b) =
  (f, init, xs) =>
    snd(
      foldLeft(
        ((acc, result), curr) => {
          let nextAcc = f(acc, curr);
          (nextAcc, append(nextAcc, result));
        },
        (init, [||]),
        xs,
      ),
    );

let scanRight: (('a, 'b) => 'b, 'b, array('a)) => array('b) =
  (f, init, xs) =>
    snd(
      foldRight(
        (curr, (acc, result)) => {
          let nextAcc = f(curr, acc);
          (nextAcc, prepend(nextAcc, result));
        },
        (init, [||]),
        xs,
      ),
    );

let get: (int, array('a)) => option('a) = (i, xs) => Belt.Array.get(xs, i);

let set: (int, 'a, array('a)) => bool =
  (i, x, xs) => Belt.Array.set(xs, i, x);

let head: array('a) => option('a) = arr => Belt.Array.get(arr, 0);

let tail: array('a) => option(array('a)) =
  xs => {
    let l = length(xs);
    if (l == 0) {
      None;
    } else if (l == 1) {
      Some([||]);
    } else {
      let ys = Belt.Array.sliceToEnd(xs, 1);
      Belt.Array.length(ys) > 0 ? Some(ys) : None;
    };
  };

let tailOrEmpty: array('a) => array('a) =
  arr => tail(arr)->Belt.Option.getWithDefault(empty);

let init: array('a) => option(array('a)) =
  xs => {
    let l = length(xs);
    if (l == 0) {
      None;
    } else {
      Some(Belt.Array.slice(xs, ~offset=0, ~len=l - 1));
    };
  };

let last: array('a) => option('a) =
  xs => {
    let l = length(xs);
    if (l == 0) {
      None;
    } else {
      get(l - 1, xs);
    };
  };

let take: (int, array('a)) => option(array('a)) =
  (i, xs) =>
    if (i > length(xs)) {
      None;
    } else {
      Some(Belt.Array.slice(xs, ~offset=0, ~len=i));
    };

let takeUpTo: (int, array('a)) => array('a) =
  (i, xs) => Belt.Array.slice(xs, ~offset=0, ~len=i);

let rec takeWhile: ('a => bool, array('a)) => array('a) =
  (f, xs) =>
    switch (head(xs)) {
    | Some(x) when f(x) => prepend(x, takeWhile(f, tailOrEmpty(xs)))
    | _ => [||]
    };

let drop: (int, array('a)) => option(array('a)) =
  (i, xs) =>
    if (i > length(xs)) {
      None;
    } else {
      Some(Belt.Array.sliceToEnd(xs, i));
    };

let dropUpTo: (int, array('a)) => array('a) =
  (i, xs) => {
    let l = length(xs);
    Belt.Array.sliceToEnd(
      xs,
      if (i > l) {
        l;
      } else {
        i;
      },
    );
  };

let rec dropWhile: ('a => bool, array('a)) => array('a) =
  (f, xs) =>
    switch (head(xs)) {
    | Some(x) when f(x) => dropWhile(f, tailOrEmpty(xs))
    | _ => xs
    };

let filter: ('a => bool, array('a)) => array('a) =
  (f, xs) => Belt.Array.keep(xs, f);

let filterWithIndex: (('a, int) => bool, array('a)) => array('a) =
  (f, xs) => Belt.Array.keepWithIndex(xs, f);

let rec find: ('a => bool, array('a)) => option('a) =
  (f, xs) =>
    switch (head(xs)) {
    | Some(x) when f(x) => Some(x)
    | None => None
    | _ => find(f, tailOrEmpty(xs))
    };

let findWithIndex: (('a, int) => bool, array('a)) => option('a) =
  (f, xs) => {
    let rec go = (f, ys, i) =>
      switch (head(ys)) {
      | Some(y) when f(y, i) => Some(y)
      | None => None
      | _ => go(f, tailOrEmpty(ys), i + 1)
      };
    go(f, xs, 0);
  };

let partition: ('a => bool, array('a)) => (array('a), array('a)) =
  (f, xs) => Belt.Array.partition(xs, f);

let splitAt: (int, array('a)) => option((array('a), array('a))) =
  (i, xs) =>
    if (i > length(xs)) {
      None;
    } else {
      Some((
        Belt.Array.slice(xs, ~offset=0, ~len=i),
        Belt.Array.sliceToEnd(xs, i),
      ));
    };

let rec prependToAll: ('a, array('a)) => array('a) =
  (delim, xs) =>
    switch (head(xs)) {
    | None => [||]
    | Some(x) => concat([|delim, x|], prependToAll(delim, tailOrEmpty(xs)))
    };

let intersperse: ('a, array('a)) => array('a) =
  (delim, xs) =>
    switch (head(xs)) {
    | None => [||]
    | Some(x) => prepend(x, prependToAll(delim, tailOrEmpty(xs)))
    };

let replicate: (int, array('a)) => array('a) =
  (i, xs) =>
    foldLeft((acc, _i) => concat(acc, xs), [||], Relude_Int.rangeAsArray(0, i));

let zip: (array('a), array('b)) => array(('a, 'b)) = Belt.Array.zip;

let zipWith: (('a, 'b) => 'c, array('a), array('b)) => array('c) =
  (f, xs, ys) => Belt.Array.zipBy(xs, ys, f);

let zipWithIndex: array('a) => array(('a, int)) =
  xs => zip(xs, Relude_Int.rangeAsArray(0, length(xs)));

let unzip: array(('a, 'b)) => (array('a), array('b)) = Belt.Array.unzip;

let sortWithInt: (('a, 'a) => int, array('a)) => array('a) =
  (f, xs) => Belt.SortArray.stableSortBy(xs, f);

let sort: (('a, 'a) => BsAbstract.Interface.ordering, array('a)) => array('a) =
  (f, xs) => sortWithInt((a, b) => f(a, b) |> Relude_Ordering.toInt, xs);

let shuffleInPlace: array('a) => array('a) =
  xs => {
    Belt.Array.shuffleInPlace(xs);
    xs;
  };

let reverse: array('a) => array('a) = Belt.Array.reverse;

let any: ('a => bool, array('a)) => bool =
  (f, xs) => Belt.Array.some(xs, f);

let contains: (('a, 'a) => bool, 'a, array('a)) => bool =
  (f, x, xs) => any(f(x), xs);

let indexOf: (('a, 'a) => bool, 'a, array('a)) => option(int) =
  (f, x, xs) => {
    let rec go = (f, ys, i) =>
      switch (head(ys)) {
      | None => None
      | Some(z) when f(x, z) => Some(i)
      | _ => go(f, tailOrEmpty(ys), i + 1)
      };
    go(f, xs, 0);
  };

let all: ('a => bool, array('a)) => bool =
  (f, xs) => Belt.Array.every(xs, f);

/* TODO: distinct function that uses ordering so we can use a faster Set (Belt.Set?) to check for uniqueness */

let distinct: (('a, 'a) => bool, array('a)) => array('a) =
  (eq, xs) =>
    foldLeft(
      /* foldRight would probably be faster with cons, but you lose the original ordering on the list */
      (acc, curr) =>
        if (contains(eq, curr, acc)) {
          acc;
        } else {
          append(curr, acc);
        },
      [||],
      xs,
    );

let map: ('a => 'b, array('a)) => array('b) = BsAbstract.Array.Functor.map;

let mapWithIndex: (('a, int) => 'b, array('a)) => array('b) =
  (f, xs) => Belt.Array.mapWithIndex(xs, f);

let forEach: ('a => unit, array('a)) => unit =
  (f, xs) => Belt.Array.forEach(xs, f);

let forEachWithIndex: (('a, int) => unit, array('a)) => unit =
  (f, xs) => Belt.Array.forEachWithIndex(xs, f);

let apply: (array('a => 'b), array('a)) => array('b) = BsAbstract.Array.Apply.apply;

let bind: (array('a), 'a => array('b)) => array('b) = BsAbstract.Array.Monad.flat_map;

let flatMap: ('a => array('b), array('a)) => array('b) = (f, fa) => bind(fa, f);

let flatten: array(array('a)) => array('a) = Belt.Array.concatMany;

let fromList: list('a) => array('a) = Belt.List.toArray;

let toList: array('a) => list('a) = Belt.List.fromArray;

let mkString: (string, array(string)) => string =
  (delim, xs) => {
    let delimited = intersperse(delim, xs);
    foldLeft((acc, curr) => acc ++ curr, "", delimited);
  };

module Eq = BsAbstract.Array.Eq;

let rec eqBy: (('a, 'a) => bool, array('a), array('a)) => bool =
  (innerEq, xs, ys) =>
    switch (head(xs), head(ys)) {
    | (None, None) => true
    | (Some(x), Some(y)) when innerEq(x, y) =>
      eqBy(innerEq, tailOrEmpty(xs), tailOrEmpty(ys))
    | _ => false
    };

let eq =
    (
      type a,
      eqA: (module BsAbstract.Interface.EQ with type t = a),
      xs: array(a),
      ys: array(a),
    )
    : bool => {
  module EqA = (val eqA);
  eqBy(EqA.eq, xs, ys);
};

module Show = BsAbstract.Array.Show;

let showBy: ('a => string, array('a)) => string =
  (showX, xs) => {
    let strings = map(showX, xs);
    "[" ++ mkString(", ", strings) ++ "]";
  };

let show =
    (
      type a,
      showA: (module BsAbstract.Interface.SHOW with type t = a),
      xs: array(a),
    )
    : string => {
  module ShowA = (val showA);
  showBy(ShowA.show, xs);
};

module SemigroupAny:
  BsAbstract.Interface.SEMIGROUP_ANY with type t('a) = array('a) = {
  type t('a) = array('a);
  let append = concat;
};

module MonoidAny: BsAbstract.Interface.MONOID_ANY with type t('a) = array('a) = {
  include SemigroupAny;
  let empty = empty;
};

module Functor = BsAbstract.Array.Functor;

module Apply = BsAbstract.Array.Apply;

module Applicative = BsAbstract.Array.Applicative;

module Monad = BsAbstract.Array.Monad;

module Alt = BsAbstract.Array.Alt;

module Plus = BsAbstract.Array.Plus;

module Alternative = BsAbstract.Array.Alternative;

module Foldable = BsAbstract.Array.Foldable;

module Traversable = BsAbstract.Array.Traversable;

module Ord = BsAbstract.Array.Ord;

module Invariant = BsAbstract.Array.Invariant;

module MonadZero = BsAbstract.Array.Monad_Zero;

module MonadPlus = BsAbstract.Array.Monad_Plus;

module Extend = BsAbstract.Array.Extend;

module Infix = BsAbstract.Array.Infix;

module Sequence: Relude_Sequence.SEQUENCE with type t('a) = array('a) = {
  type t('a) = array('a);

  let length = length;
  let isEmpty = isEmpty;
  let isNotEmpty = isNotEmpty;
  let head = head;
  let tail = tail;
  let tailOrEmpty = tailOrEmpty;
  let eqBy = eqBy;
  let showBy = showBy;
  let mkString = mkString;

  module SemigroupAny = SemigroupAny;
  module MonoidAny = MonoidAny;
  module Functor = Functor;
  module Apply = Apply;
  module Applicative = Applicative;
  module Monad = Monad;
  module Foldable = Foldable;
  module Traversable = Traversable;
  module Eq = Eq;
  module Show = Show;
};

module IsoList: Relude_IsoList.ISO_LIST with type t('a) = array('a) = {
  type t('a) = array('a);
  let fromList = fromList;
  let toList = toList;
};