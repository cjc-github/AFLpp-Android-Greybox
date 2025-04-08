package qb.blogfuzz;

import java.util.Arrays;

public class Wrapper {
    private final byte[] mBuffer;

    public Wrapper(byte[] buffer) {
        this.mBuffer = buffer;
    }

    public byte[] getBuffer() {
        byte[] bArr = this.mBuffer;
        return Arrays.copyOf(bArr, bArr.length);
    }
}
