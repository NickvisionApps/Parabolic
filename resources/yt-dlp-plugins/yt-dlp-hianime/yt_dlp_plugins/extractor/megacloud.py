import base64
import re
import requests
from typing import Callable, Iterable, TypeVar, overload, Literal, TypeAlias, Any
from enum import StrEnum, IntFlag

DEFAULT = object()
HEXDIGITS = "0123456789abcdef"

T = TypeVar("T")
_KeyPair: TypeAlias = tuple[list[str], list[int]]


@overload
def _re(pattern: "Patterns", string: str) -> re.Match: ...
@overload
def _re(pattern: "Patterns", string: str, *, default: T) -> re.Match | T: ...
@overload
def _re(pattern: "Patterns", string: str, *, all: Literal[True]) -> list: ...
@overload
def _re(pattern: "Patterns", string: str, *, all: Literal[True], default: T) -> list | T: ...


def _re(pattern: "Patterns", string: str, *, all: bool = False, default: T = DEFAULT) -> re.Match | list | T:
    v = re.findall(pattern.formatted, string) if all else re.search(pattern.formatted, string)

    if not v and default is DEFAULT:
        msg = f"{pattern.name} not found"
        raise ValueError(msg)

    elif not v:
        return default

    return v

def make_request(url: str, headers: dict, params: dict, func: Callable[[requests.Response], Any]) -> Any:
    """
    Makes a synchronous HTTP GET request using the requests library.
    """
    try:
        with requests.get(url, headers=headers, params=params) as resp:
            resp.raise_for_status()  # Raise an exception for bad status codes (4xx or 5xx)
            return func(resp)
    except requests.exceptions.RequestException as e:
        print(f"An error occurred during the request: {e}")
        return None


def hash(key: str) -> int:
    key_value = 0
    for char in key:
        key_value = (key_value * 31 + ord(char)) & 0xFFFFFFFF

    return key_value


def hash_float(key: str) -> float:
    result = 0
    m = int((16 * len(key) - 863) / 15)

    for char in key:
        result = ord(char) + result * (m + 127)

    return float(result % 0x7FFFFFFFFFFFFFFF)


def compute_xor_value(key_len: int) -> int:
    base = 81
    max_val = 255
    k = (max_val - base) // key_len
    return base + key_len * k


def arr_split(s):
    parts = []
    current = []
    depth = 0
    for char in s:
        if char == "," and depth == 0:
            parts.append("".join(current).strip())
            current = []
        else:
            if char == "(":
                depth += 1
            elif char == ")":
                depth -= 1
            current.append(char)
    if current:
        parts.append("".join(current).strip())
    return parts


def generate_index_sequence(n: int) -> list[int]:
    result = [5, 8, 14, 11]
    if n <= 4:
        return result

    for i in range(2, n - 2):
        result.append(result[i] + i + 3 - (i % 2))

    return result


class ResolverFlags(IntFlag):
    FALLBACK = 1
    REVERSE = 1 << 1
    FROMCHARCODE = 1 << 2
    SLICE = 1 << 3
    SPLIT = 1 << 4
    ABC = 1 << 5


class Patterns(StrEnum):
    _FUNC = r"[\w$]{3}\.[\w$]{2}"
    _FUNC2 = r"[\w$]\.[\w$]{2}"
    _FUNC3 = r"[\w$]{3}\.[\w$]{3}"

    BIGINT = f"12345n"

    CLIENT_KEY = r'([a-zA-Z0-9]{48})|x: "([a-zA-Z0-9]{16})", y: "([a-zA-Z0-9]{16})", z: "([a-zA-Z0-9]{16})"};'
    SOURCE_ID = r"embed-2/v3/e-1/([a-zA-Z0-9]+)\?"
    SOURCES = r'(\[.+?"hls"}\])'

    IDX = r'(?<=[\(",])(\d+)'
    VAR = r"(?:^|[ ;{])%%name%%=([^;]+);|[\(,]%%name%% = ([^\)]+)\)"
    DICT = r"[\w$]{2}=\{\}"

    XOR_KEY = r"\)\('(.+)'\)};"
    STRING = r"function [\w$]{2}\(\){return \"(.+?)\";}"
    DELIMITER = r"[\w$]{3}=\w\.[\w$]{2}\([\w$]{3},'(.)'\);"

    SET_DEFAULT_OPCODE = rf"{_FUNC}\(([0-9]|1[0-6])\)"

    COMPUTE_OP_FUNC = r"\w\[\d+\]=\(function\([\w$]+\)[{\d\w$:\(\),= ]+;switch\([\w$]+\){([^}]+)}"
    OPERATION = r"case (\d+):([\w\[\]\-+|><^*\/&% =$\(\)]+);break;"

    SLICES = rf"case\s(\d{{1,2}}):{_FUNC2}\({_FUNC2}\(\),[\w$]{{3}},{_FUNC2}\({_FUNC2}\([\w$]{{3}},([\d\-]+),[\d\-]+\),[\d\-]+,([\d\-]+)\)\)"

    _GET_INDEX = r'+?"?([\w$]+)"?'
    _GET1_INDEX = rf'{_GET_INDEX}( [|\-\+*><^]+ "?[\w$]+"?)?'
    GET1 = rf"{_FUNC}\(\{_GET1_INDEX}\)"
    GET2 = rf"{_FUNC}\({_FUNC}\({_GET_INDEX},{_GET_INDEX}\)\)"
    GET3 = rf"{_FUNC}\({_FUNC}\({_GET_INDEX},{_GET_INDEX},{SET_DEFAULT_OPCODE}\)\)"
    GET4 = rf"{_FUNC}\({_GET_INDEX},{_GET_INDEX},{_GET_INDEX},{_GET_INDEX}\)"
    GET5 = rf"{_FUNC}\({_GET_INDEX},{_GET_INDEX},{_GET_INDEX},{_GET_INDEX},{_GET_INDEX}\)"
    GET6 = rf"{_FUNC}\({_GET_INDEX},{_GET_INDEX},{_GET_INDEX},{_GET_INDEX},{_GET_INDEX},{_GET_INDEX}\)"
    GET = f"({GET1}|{GET2}|{GET3}|{GET4}|{GET5}|{GET6})"

    ARRAY_CONTENT = r';\w=\[((?!arguments)[\w\d.$\(\)",+]+)\];'
    KEY_VAR = r"var (?:[\w$]{1,2},){28,}.+?[\w$\.]+=([^;]+?);"

    PARSE_INT = r'[\w$]+\({%%value%%},\+?"?16"?'
    APPLY_OP = rf"{_FUNC}\((\w+),(\w+)\)"
    APPLY_OP_SPEC = rf'{_FUNC}\("?(\d+)"?,"?(\d+)"?,{_FUNC}\((\d)\)\)'

    GET_KEY_CTX = rf"var (?:[\w$]{{1,2}},?){{28,}};.+?({SET_DEFAULT_OPCODE};\w=.+?)try"
    GET_KEY_FUNC = r"(\w)=\(\)=>{(.+?)};"
    GET_KEY_FUNC_RETURN = r"return(.+?);[\}\)]"
    GET_KEY_FUNC_MAP = r"\((\w+)=>{(.+?return.+?;)"

    DICT_SET1 = rf"[\w$]{{2}}\[(?:{GET})\]=({GET})"
    DICT_SET2 = rf"[\w$]{{2}}\[(?:{GET})\]=\(\)=>({{.+?return {GET})"
    DICT_SET = f"{DICT_SET1}|{DICT_SET2}"

    KEY_TRANSFORM_CTX = rf"[A-Za-z]=\([\w$]{{2}},[\w$]{{2}}\)=>{{if\({_FUNC}\(\)\){{var (?:[\w$]{{2}},?){{12}}(.+?return)"
    KEY_TRANSFORM_MULTIPLIER = r'[\w$]{2}=[\w$]{5}\(\+?"?(\d+)"?\)'
    KEY_TRANSFORM_XOR_VALUE = r'[\w$]{2}=[\w$]{5}\(\+?"?(\d+)"?\)'
    KEY_TRANSFORM_SUMMAND = r'[\w$]{2} % [\w$]{2}\[.+?"(\d+)"'

    def fmt(self, **kwargs) -> "Patterns":
        for k, v in kwargs.items():
            v = re.escape(v)
            self._fmted = self.value.replace(f"%%{k}%%", v)

        return self

    @property
    def formatted(self) -> str:
        return getattr(self, "_fmted", self.value)


class KeyResolver:
    @staticmethod
    def _get_key(s: "Megacloud") -> str:
        fcall = _re(Patterns.KEY_VAR, s.script).group(1)
        args = _re(Patterns.GET, fcall).groups()

        return s._get(args[1:], fcall).replace("-", "")

    @staticmethod
    def _get_keys(s: "Megacloud") -> list[str]:
        array_items = _re(Patterns.ARRAY_CONTENT, s.script, all=True)[0]
        array_items = arr_split(array_items)
        keys = []

        if any(i.isdigit() for i in array_items):
            return keys

        for fcall in array_items:
            args = _re(Patterns.GET, fcall).groups()
            keys.append(s._get(args[1:], ""))

        return keys

    @staticmethod
    def _get_indexes(s: "Megacloud") -> list[int]:
        array_items = _re(Patterns.ARRAY_CONTENT, s.script, all=True)[-1]
        array_items: list[str] = arr_split(array_items)
        ctx = _re(Patterns.GET_KEY_CTX, s.script).group(1)
        indexes = []

        if not any(i.isdigit() for i in array_items):
            return indexes

        for item in array_items:
            args = _re(Patterns.IDX, item, all=True, default=[item])

            if len(args) > 1:
                if len(args) == 2:
                    idx = s._apply_op(args, ctx=ctx)

                else:
                    idx = s._apply_op(args, opcode=int(args[2]))

            else:
                idx = args[0]

            indexes.append(int(idx))

        return indexes

    @classmethod
    def map(cls, s: "Megacloud") -> _KeyPair:
        try:
            keys = cls._get_keys(s)
        except ValueError:
            keys = []

        try:
            indexes = cls._get_indexes(s)
        except ValueError:
            indexes = []

        return keys, indexes

    @classmethod
    def slice(cls, s: "Megacloud") -> _KeyPair:
        key = cls._get_key(s)

        if any(c not in HEXDIGITS for c in key):
            key = base64.b64decode(key).decode()

        return list(key), list(range(0, len(key)))

    @classmethod
    def abc(cls, s: "Megacloud") -> _KeyPair:
        values = {}
        ctx = _re(Patterns.GET_KEY_CTX, s.script).group(1)

        for f in _re(Patterns.DICT_SET, ctx, all=True):
            i = 0 if f[0] else 17
            key_idxs = list(filter(None, f[i + 1 : i + 8]))

            context = f[i + 8]
            value_idxs = list(filter(None, f[i + 10 : i + 17]))

            k = s._get(key_idxs, ctx)
            v = s._get(value_idxs, context)

            values[k] = v

        get_key_func = _re(Patterns.GET_KEY_FUNC, ctx).group(2)

        order = get_key_func.split("return")[-1].split(";")[0]
        order = order.replace("()", "")
        order = re.sub(rf"\w\[(.+?)\]", r"\1", order)

        for f in _re(Patterns.GET, order, all=True):
            indexes = list(filter(None, f[1:]))

            v = s._get(indexes, get_key_func)
            order = order.replace(f[0], f'"{values[v]}"')

        key = eval(order)
        return list(key), list(range(0, len(key)))

    @classmethod
    def add_funcs(cls, s: "Megacloud") -> _KeyPair:
        ctx = _re(Patterns.GET_KEY_CTX, s.script).group(1)
        funcs = _re(Patterns.GET_KEY_FUNC, ctx, all=True)

        if len(funcs) < 3:
            return [], []

        key = ""

        for f in funcs[:-1]:
            ret = _re(Patterns.GET_KEY_FUNC_RETURN, f[1]).group(1)
            args = _re(Patterns.GET, ret).groups()

            key += s._get(args[1:], f[1])

        return list(key), list(range(0, len(key)))

    @classmethod
    def from_charcode(cls, s: "Megacloud", keys: list = [], indexes: list = []) -> _KeyPair:
        raw_values = []
        ctx = _re(Patterns.GET_KEY_CTX, s.script).group(1)

        if indexes:
            raw_values = indexes

            map_ = _re(Patterns.GET_KEY_FUNC_MAP, ctx, default=None)
            if map_:
                map_arg = map_.group(1)
                map_body = map_.group(2)

                apply_op = _re(Patterns.APPLY_OP, map_body)
                opcode = _re(Patterns.SET_DEFAULT_OPCODE, map_body).group(1)

                var_name = apply_op.group(1) if apply_op.group(1) != map_arg else apply_op.group(2)
                var_value = s._var_to_num(var_name, s.script)

                raw_values = [s._apply_op((var_value, i), opcode=int(opcode)) for i in raw_values]

        elif keys:
            map_ = _re(Patterns.GET_KEY_FUNC_MAP, ctx)
            map_arg = map_.group(1)
            map_body = map_.group(2)

            if _re(Patterns.PARSE_INT.fmt(value=map_arg), map_body, default=None):
                raw_values = [int(k, 16) for k in keys]

        else:
            indexes = cls._get_indexes(s)
            raw_values = [int(i) for i in indexes]

        return [chr(v) for v in raw_values], list(range(0, len(raw_values)))

    @classmethod
    def compute_strings(cls, s: "Megacloud") -> _KeyPair:
        ctx = _re(Patterns.GET_KEY_CTX, s.script).group(1)
        ret = _re(Patterns.GET_KEY_FUNC_RETURN, ctx).group(1)

        apply_op_args = _re(Patterns.APPLY_OP, ret)
        a, b = apply_op_args.group(1), apply_op_args.group(2)

        a_get = _re(Patterns.VAR.fmt(name=a), ctx).group(1)
        b_get = _re(Patterns.VAR.fmt(name=b), ctx).group(1)

        a_get_args = _re(Patterns.GET, a_get).groups()[1:]
        b_get_args = _re(Patterns.GET, b_get).groups()[1:]

        a_value = s._get(a_get_args, ctx)
        b_value = s._get(b_get_args, ctx)

        if any(c not in HEXDIGITS for c in a_value):
            a_value = base64.b64decode(a_value).decode()

        if any(c not in HEXDIGITS for c in b_value):
            b_value = base64.b64decode(b_value).decode()

        ctx = _re(Patterns.GET_KEY_FUNC, ctx).group(2)
        opcode = _re(Patterns.SET_DEFAULT_OPCODE, ctx).group(1)

        key = s.compute_op[int(opcode)](a_value, b_value)

        return list(key), list(range(0, len(key)))

    @classmethod
    def fallback(cls, s: "Megacloud", keys: list, indexes: list) -> _KeyPair:
        def _map(_) -> _KeyPair:
            if keys and indexes:
                key = "".join(keys[i] for i in indexes)
                if len(key) == 64:
                    return keys, indexes

            return [], []

        to_try = [_map, cls.compute_strings, cls.slice, cls.add_funcs, cls.from_charcode]

        for func in to_try:
            try:
                result = func(s)
                if result[0]:
                    return result
                continue

            except ValueError:
                continue

        return [], []

    @classmethod
    def resolve(cls, flags: ResolverFlags, s: "Megacloud") -> str:
        key = ""
        keys, indexes = cls.map(s)

        if flags & (ResolverFlags.SLICE | ResolverFlags.SPLIT):
            keys, indexes = cls.slice(s)

        if flags & ResolverFlags.FROMCHARCODE:
            keys, indexes = cls.from_charcode(s, keys, indexes)

        if flags & ResolverFlags.ABC:
            keys, indexes = cls.abc(s)

        if flags & ResolverFlags.FALLBACK:
            keys, indexes = cls.fallback(s, keys, indexes)

        key = [keys[i] for i in indexes]

        if flags & ResolverFlags.REVERSE:
            key = reversed(key)

        return "".join(key)


class KeyTransform:
    def __init__(self, secret_key: str, client_key: str, script: str) -> None:
        self.secret_key = secret_key
        self.client_key = client_key
        self.script = script
        self.key = secret_key + client_key

    def __iter__(self):
        self.__c = 4
        return self

    def __next__(self):
        if self.__c == 1:
            raise StopIteration

        self.__c -= 1
        return self.__c

    def apply(self) -> str:
        # apply only if it wasnt applied before
        if self.key == self.secret_key + self.client_key:
            self.key = self._apply()

        return f"{self.key}{self.__c}"

    def _apply(self) -> str:
        result = []
        client_key = list(reversed(self.client_key))

        key_hash = hash_float(self.key)

        xor_value = compute_xor_value(len(self.key))
        key = [chr(ord(char) ^ xor_value) for char in self.key]

        summand = int(_re(Patterns.KEY_TRANSFORM_SUMMAND, self.script).group(1))
        slice1 = int(key_hash % len(self.key)) + summand
        key = key[slice1:] + key[:slice1]

        for i, char in enumerate(key):
            result.append(char)
            if i < len(client_key):
                result.append(client_key[i])

        slice2 = int(key_hash % 33) + 96
        result = map(lambda char: chr((ord(char) % 95) + 32), result[:slice2])

        return "".join(result)

class Megacloud:
    base_url = "https://megacloud.blog"
    headers = {
        "user-agent": "Mozilla/5.0 (X11; Linux x86_64; rv:139.0) Gecko/20100101 Firefox/139.0",
        "origin": base_url,
        "referer": base_url + "/",
    }
    BIGINT_NUMBERS = False

    def __init__(self, embed_url: str) -> None:
        self.embed_url = embed_url

        self.script: str
        self.string_array: list[str]
        self.compute_op: dict[int, Callable]

    def _convert_to_js_operation(self, operation: str) -> str:
        operand = r"\([\w$ *>^+&\[\]]+\)|[\w$]+\[\d\]|int\(.+?\)"
        multi = rf"({operand}) (\*|\/|\+|-) ({operand})"
        shift = rf"({operand}) (>>|<<) ({operand})"

        if not self.BIGINT_NUMBERS:
            while re.search(multi, operation):
                operation = re.sub(multi, r"int(float(\1) \2 float(\3))", operation)

        while re.search(shift, operation):
            operation = re.sub(shift, r"\1 \2 (\3 & 31)", operation)

        return operation

    def _generate_op_func(self, operation: str) -> Callable:
        operation = re.sub(r"[\w$]{2}", "args", operation)
        string = self._convert_to_js_operation(operation)
        return lambda *args: eval(string)

    def _get_operations(self) -> dict[int, Callable]:
        functions = {}

        compute_op_func = _re(Patterns.COMPUTE_OP_FUNC, self.script).group(1)
        for num, operation in _re(Patterns.OPERATION, compute_op_func, all=True):
            functions[int(num)] = self._generate_op_func(operation.split("=")[1])

        return functions

    def _get_array_slices(self) -> list[tuple[int, ...]]:
        pairs = tuple(map(lambda t: tuple(map(int, t)), _re(Patterns.SLICES, self.script, all=True)))
        order_map = {v: i for i, v in enumerate(generate_index_sequence(len(pairs)))}

        pairs = list(sorted(pairs, key=lambda t: order_map[t[0]]))
        return pairs

    def _shuffle_array(self, array: list[str]) -> list[str]:
        slices = self._get_array_slices()
        for _, array_idx, tail_idx in slices:
            array, tail = array[:array_idx], array[array_idx:]
            array = tail[:tail_idx] + array

        return array

    def _get_opcodes(self, ctx: str) -> list[int]:
        try:
            opcodes = _re(Patterns.SET_DEFAULT_OPCODE, ctx, all=True)
            opcodes = list(filter(lambda i: i <= 15, map(int, opcodes)))

        except ValueError:
            opcodes = [0]

        return opcodes

    def _apply_op(self, args: Iterable, *, ctx: str | None = None, opcode: int | None = None) -> int:
        args = list(args)
        for i, arg in enumerate(args):
            if isinstance(arg, str):
                arg = arg.rstrip("n")
                if arg.startswith("0x"):
                    args[i] = int(arg, 16)

                else:
                    args[i] = int(arg)

            else:
                args[i] = int(arg)

        if opcode is not None:
            return int(self.compute_op[opcode](*args))

        elif ctx is None:
            raise SyntaxError

        for o in self._get_opcodes(ctx):
            try:
                v = int(self.compute_op[o](*args))

            except IndexError:
                continue

            if v in range(0, len(self.string_array)):
                return v

        raise ValueError(f"can't apply op")

    def _var_to_num(self, var: str, ctx: str) -> str:
        if not var.isdigit():
            var_name = var.replace("$", r"\$")

            var_value = _re(Patterns.VAR.fmt(name=var_name), self.script)
            var_value = var_value.group(1) or var_value.group(2)
            var_value = re.sub(Patterns._FUNC, "", var_value)

            if 0 < len(var_value) < 4 and not var_value.isdigit():
                return self._var_to_num(var_value, ctx)

            digits = re.findall(r"\d+", var_value)
            assert len(digits) > 0

            if len(digits) == 1:
                return str(digits[0])

            result = self._apply_op(digits, ctx=ctx)

            if not result:
                var_value = " ".join(map(lambda t: m.group(1) if (m := re.search(r"(\d+)", t)) else t, var_value.split()))
                result = eval(self._convert_to_js_operation(var_value))

            return str(result)

        return var

    def _get(self, values, ctx: str) -> str:
        values = list(filter(None, values))

        if len(values) == 1:
            i = int(self._var_to_num(values[0], ctx))
            return self.string_array[i]

        elif len(values) > 1:
            if not values[1].isdigit():
                i = eval(self._convert_to_js_operation(" ".join(values)))

            else:
                i1 = int(self._var_to_num(values[0], ctx))
                i2 = int(self._var_to_num(values[1], ctx))

                if len(values) == 3:
                    opcode = int(self._var_to_num(values[2], ctx))

                else:
                    opcode = None

                i = self._apply_op((i1, i2), ctx=ctx, opcode=opcode)

            return self.string_array[i]

        raise ValueError(f"can't get {values}")

    def _resolve_secret_key(self) -> str:
        ctx = _re(Patterns.GET_KEY_CTX, self.script).group(1)
        get_key_body = _re(Patterns.GET_KEY_FUNC, ctx).group(2)

        functions: list[str] = []

        for i in _re(Patterns.GET, get_key_body, all=True, default=[]):
            if not _re(Patterns.SET_DEFAULT_OPCODE, i[0], default=None):
                string = self._get(i[1:], get_key_body)
                functions.append(string)

        flags = ResolverFlags(0)

        for f in functions:
            if f.upper() in ResolverFlags._member_names_:
                flags |= ResolverFlags[f.upper()]

            elif len(f) == 1 and ord(f) in range(97, 123):
                flags |= ResolverFlags.ABC

        if not flags:
            flags = ResolverFlags.FALLBACK

        return KeyResolver.resolve(flags, self)

    def _lcg(self, n: int) -> int:
        # linear congruential generator ??
        if self.BIGINT_NUMBERS:
            return (n * 1103515245 + 12345) & 0x7FFFFFFF

        else:
            return int(float(n) * 1103515245.0 + 12345.0) & 0x7FFFFFFF

    def _shuffle_sources(self, sources: list[str], key: str) -> list[str]:
        array_count = len(sources) // len(key)
        arrays = [[""] * len(key) for _ in range(array_count)]

        key_dict = {i: char for i, char in enumerate(key)}
        key_sorted = {i: char for i, char in sorted(key_dict.items(), key=lambda p: p[1])}

        p = 0
        for idx in key_sorted.keys():
            for arr_idx in range(array_count):
                char = sources[p]
                arrays[arr_idx][idx] = char
                p += 1

        sources = []
        for arr in arrays:
            sources.extend(arr)

        return sources

    def _shuffle_key(self, key: str) -> str:
        key_hash = hash(key)
        shuffled_key = [chr(c) for c in range(32, 127)]

        for i in reversed(range(len(shuffled_key))):
            key_hash = self._lcg(key_hash)
            mod = key_hash % (i + 1)

            shuffled_key[i], shuffled_key[mod] = shuffled_key[mod], shuffled_key[i]

        return "".join(shuffled_key)

    def _process_sources(self, sources: list[str], key: str) -> list[str]:
        current_hash = hash(key)
        new_sources = []

        for char in sources:
            current_hash = self._lcg(current_hash)

            val1 = ord(char) - 32
            val2 = current_hash % 95

            v = (val1 - val2) % 95 + 32
            new_char = chr(v)
            new_sources.append(new_char)

        return self._shuffle_sources(new_sources, key)

    def _extract_client_key(self) -> str:
        resp = make_request(self.embed_url, self.headers, {}, lambda r: r.text)

        if resp is None:
            raise ValueError("Failed to retrieve client key from embed URL")

        meta_parts = filter(None, _re(Patterns.CLIENT_KEY, resp).groups())
        return "".join(meta_parts)

    def extract(self) -> dict:
        id = _re(Patterns.SOURCE_ID, self.embed_url).group(1)

        client_key = self._extract_client_key()
        get_src_url = f"{self.base_url}/embed-2/v3/e-1/getSources"
        resp = make_request(get_src_url, self.headers, {"id": id, "_k": client_key}, lambda i: i.json())

        if resp is None:
            raise ValueError("Failed to get sources from getSources URL")

        resp["intro"] = resp["intro"]["start"], resp["intro"]["end"]
        resp["outro"] = resp["outro"]["start"], resp["outro"]["end"]

        return resp