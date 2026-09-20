# JSON Parser and Serializer for y-- (Ymm)

High-performance JSON serialization and deserialization for y-- powered by nlohmann/json.

## Installation

This library is an official external module for the **y--** (**Ymm**) programming language. You can install it effortlessly via **`yenv`**:

```bash
yenv install json
```

This will automatically clone the repository, build the shared library (`liby-json.so` / `y-json.dll`), and register it in `~/.yenv/lib/`.

## Usage

```ymm
import Json
from Json import *

define function main(): {
    data = Json.parse("{\"name\": \"Alice\", \"score\": 100}")
    print(data["name"])
    print(Json.stringify(data))
}
```

## Documentation

Full documentation, including manual pages and reference texts, are available in the [`docs/`](docs/) directory:
- `json.man`: UNIX troff man page
- `json.txt`: Plaintext documentation reference

You can view the documentation using:
```bash
man -l docs/json.man
```

## License

This project is licensed under the same license as the **y--** programming language. See the [LICENSE](LICENSE) file for details.
