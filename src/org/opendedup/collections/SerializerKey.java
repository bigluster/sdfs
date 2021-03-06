package org.opendedup.collections;

import org.mapdb.*;
import org.mapdb.serializer.GroupSerializer;
import org.opendedup.hashing.HashFunctionPool;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.Comparator;

/**
 * Created by jan on 2/28/16.
 */
public class SerializerKey implements GroupSerializer<byte[]> {


    @Override
    public void serialize(DataOutput2 out, byte[] value) throws IOException {
        //out.packInt(value.length);
        out.write(value);
    }

    @Override
    public byte[] deserialize(DataInput2 in, int available) throws IOException {
        //int size = in.unpackInt();
        byte[] ret = new byte[HashFunctionPool.hashLength];
        in.readFully(ret);
        return ret;
    }


    @Override
    public boolean isTrusted() {
        return true;
    }

    @Override
    public boolean equals(byte[] a1, byte[] a2) {
        return Arrays.equals(a1, a2);
    }

    public int hashCode(byte[] bytes, int seed) {
        return ByteBuffer.wrap(bytes).getInt();
    }

    @Override
    public int compare(byte[] o1, byte[] o2) {
       return 0;
    }

    @Override
    public int valueArraySearch(Object keys, byte[] key) {
        return Arrays.binarySearch((byte[][])keys, key, new SerializerKey());
    }

    @SuppressWarnings({ "unchecked", "rawtypes" })
	@Override
    public int valueArraySearch(Object keys, byte[] key, Comparator comparator) {
        //TODO PERF optimize search
        Object[] v = valueArrayToArray(keys);
        return Arrays.binarySearch(v, key, comparator);
    }

    @Override
    public void valueArraySerialize(DataOutput2 out, Object vals) throws IOException {
        byte[][] vals2 = (byte[][]) vals;
        //out.packInt(vals2.length);
        for(byte[]b:vals2){
           this.serialize(out, b);
        }
    }

    @Override
    public byte[][] valueArrayDeserialize(DataInput2 in, int size) throws IOException {
        int s = HashFunctionPool.hashLength;
        byte[][] ret = new byte[s][];
        for(int i=0;i<s;i++) {
            ret[i] = this.deserialize(in, -1);
        }
        return ret;
    }

    @Override
    public byte[] valueArrayGet(Object vals, int pos) {
        return ((byte[][])vals)[pos];
    }

    @Override
    public int valueArraySize(Object vals) {
        return ((byte[][])vals).length;
    }

    @Override
    public byte[][] valueArrayEmpty() {
        return new byte[0][];
    }

    @Override
    public byte[][] valueArrayPut(Object vals, int pos, byte[] newValue) {
        byte[][] array = (byte[][])vals;
        final byte[][] ret = Arrays.copyOf(array, array.length+1);
        if(pos<array.length){
            System.arraycopy(array, pos, ret, pos+1, array.length-pos);
        }
        ret[pos] = newValue;
        return ret;
    }

    @Override
    public byte[][] valueArrayUpdateVal(Object vals, int pos, byte[] newValue) {
        byte[][] vals2 = (byte[][]) vals;
        vals2 = vals2.clone();
        vals2[pos] = newValue;
        return vals2;
    }

    @Override
    public byte[][] valueArrayFromArray(Object[] objects) {
        byte[][] ret = new byte[objects.length][];
        for(int i=0;i<ret.length;i++){
            ret[i] = (byte[])objects[i];
        }
        return ret;
    }

    @Override
    public byte[][] valueArrayCopyOfRange(Object vals, int from, int to) {
        return Arrays.copyOfRange((byte[][])vals, from, to);
    }

    @Override
    public byte[][] valueArrayDeleteValue(Object vals, int pos) {
        byte[][] vals2 = new byte[((byte[][])vals).length-1][];
        System.arraycopy(vals,0,vals2, 0, pos-1);
        System.arraycopy(vals, pos, vals2, pos-1, vals2.length-(pos-1));
        return vals2;
    }

    @Override
    public byte[] nextValue(byte[] value) {
        value = value.clone();

        for (int i = value.length-1; ;i--) {
            int b1 = value[i] & 0xFF;
            if(b1==255){
                if(i==0)
                    return null;
                value[i]=0;
                continue;
            }
            value[i] = (byte) ((b1+1)&0xFF);
            return value;
        }
    }
}
