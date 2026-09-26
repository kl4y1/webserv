# Webserv — mnajm's Branch: HTTP Protocol Layer

## Your job
You turn raw bytes into Request structs, and Response structs into raw bytes.
No sockets. No filesystem. Pure protocol.

## Your files
```
src/http/Request.cpp    ← incremental HTTP request parser
src/http/Response.cpp   ← response serializer, status codes, MIME types
```

## Headers you own
```
include/Request.hpp    ← Request struct, RequestParser, ParseState enum
include/Response.hpp   ← Response struct, HttpStatus, MimeTypes, defaultErrorPage
```

## Headers you depend on (do not modify)
```
include/Config.hpp     ← only needed for client_max_body_size check
```

## What you must implement (see comments in each .cpp)

### Request.cpp
- `CaseInsensitiveLess::operator()` — case-insensitive header map comparator
- `RequestParser::feed()`           — append bytes, drive the state machine
- `RequestParser::reset()`          — reset for next request (keep leftover bytes)
- `RequestParser::parseRequestLine()` — extract method, path, query, version
- `RequestParser::parseHeaders()`   — parse all headers; decide body mode
- `RequestParser::parseBodyLength()`  — read exactly Content-Length bytes
- `RequestParser::parseBodyChunked()` — decode chunked transfer encoding
- `RequestParser::urlDecode()`      — decode %XX sequences and + as space

### Response.cpp
- `Response::serialize()`      — produce full HTTP/1.1 wire bytes
- `HttpStatus::reason()`       — int code → reason phrase string
- `MimeTypes::lookup()`        — file extension → MIME type string
- `defaultErrorPage()`         — minimal HTML error page when config has none

## Key rules
- The parser MUST be incremental — one recv() call never guarantees a complete request
- Never throw past the parser boundary — set PS_ERROR + statusCode instead
- `serialize()` must always include Content-Length — browsers hang without it
- Handle both Content-Length and Transfer-Encoding: chunked
- URL-decode the path before handing it to abood's handler

## Integration points
aqahwaji feeds you bytes:
```cpp
parser.feed(buf, n);
if (parser.done())  { /* aqahwaji calls handle() */ }
if (parser.error()) { /* aqahwaji sends error response */ }
```

abood builds a Response and you serialize it:
```cpp
Response resp = handle(...);       // abood fills this
std::string wire = resp.serialize(); // your code
```
