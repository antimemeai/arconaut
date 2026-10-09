"""Development fixture reader/writer for the documented BBM2 layout."""
from pathlib import Path
import struct
from decimal import Decimal
WORDS = '''label metadata time clock_id utc_ns monotonic_ns sampling_span_ns synchronization variable value source sample_id observation_status kind attempt invocation operation revision generation duration_ns input_binding retry_group ordinal field identity_scope content_observation repository requested_path path exit_code commit parents author_unix_seconds committer_unix_seconds host pid scope diagnostic_file expires_at bytes available storage_error complaint activation unknown observed unavailable variable.sample clock.domain doctrine.effective git.head git.commit native_request_field instructions git_cli operation.result provider.request provider.stream process.output invocation-v1 attempt-invocation-v1 turn-boundary result_id result_record'''.split()
def read_packet(path):
    return decode_packet(Path(path).read_bytes())

def decode_packet(data):
    if data[:4] != b'BBM\x02': raise ValueError('BBM2 required')
    pos = 4
    def take(n):
        nonlocal pos
        out = data[pos:pos+n]
        if len(out) != n: raise ValueError('truncated packet')
        pos += n
        return out
    def var():
        n = 0
        for shift in range(0, 70, 7):
            b = take(1)[0]; n |= (b & 127) << shift
            if not b & 128: return n
        raise ValueError('invalid varint')
    def value():
        tag = take(1)[0]
        if tag == 0: return None
        if tag in (1, 2): return tag == 2
        if tag in (3, 4): return var() * (-1 if tag == 4 else 1)
        if tag == 5:
            neg = take(1)[0]; z = var(); exponent = (z >> 1) ^ -(z & 1)
            limbs = [struct.unpack('<I', take(4))[0] for _ in range(var())]
            coefficient = sum(n * 10**(9*i) for i,n in enumerate(limbs))
            return Decimal((-1 if neg else 1)*coefficient).scaleb(exponent)
        if tag == 6: return take(var()).decode('utf-8')
        if tag == 7: return take(16).hex()
        if tag == 8: return WORDS[var()-1]
        if tag == 9: return [value() for _ in range(var())]
        if tag == 10:
            out = {}
            for _ in range(var()):
                key = value(); out[key] = value()
            return out
        if tag == 11: return struct.unpack('<d', take(8))[0]
        raise ValueError('unknown tag')
    out = value()
    if pos != len(data): raise ValueError('trailing bytes')
    return out

def write_packet(path, packet):
    def var(n):
        out = bytearray()
        while n >= 128: out.append((n & 127) | 128); n >>= 7
        out.append(n); return bytes(out)
    def value(v):
        if v is None: return b'\x00'
        if isinstance(v, bool): return bytes([2 if v else 1])
        if isinstance(v, int): return bytes([4 if v < 0 else 3]) + var(abs(v))
        if isinstance(v, float): return b'\x0b' + struct.pack('<d', v)
        if isinstance(v, str):
            raw = v.encode('utf-8'); return b'\x06' + var(len(raw)) + raw
        if isinstance(v, list): return b'\x09' + var(len(v)) + b''.join(value(x) for x in v)
        if isinstance(v, dict): return b'\x0a' + var(len(v)) + b''.join(value(k)+value(x) for k,x in v.items())
        raise TypeError(type(v))
    Path(path).write_bytes(b'BBM\x02' + value(packet))

def read_stream(path):
    data = Path(path).read_bytes()
    if data[:5] != b'BBMS\x01': raise ValueError('BBMS1 required')
    pos = 5; out = []
    while pos < len(data):
        if len(data)-pos < 8: raise ValueError('truncated length')
        size = struct.unpack('<Q', data[pos:pos+8])[0]; pos += 8
        if size > len(data)-pos: raise ValueError('truncated packet')
        out.append(decode_packet(data[pos:pos+size])); pos += size
    return out
