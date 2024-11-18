# c2dcczp

`c2dcczp` is a Python package for decrypting data, using a custom CCZp decryption algorithm implemented in C.

## Features

- Provides efficient CCZp decryption functionality, based on C implementation.
- Supports Python 3.6+

## Installation

Install the latest version of `c2dcczp` from PyPI:

```bash
pip install c2dcczp
```

## Usage
Here's how to use `c2dcczp` for decryption:

```python
import c2dcczp

data = b'...your encrypted data...'
key = [uint32, uint32, uint32, uint32]

c2dcczp.SetKey(key)

dec = c2dcczp.decrypt(data)
