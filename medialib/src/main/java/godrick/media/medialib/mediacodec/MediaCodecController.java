package godrick.media.medialib.mediacodec;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.view.Surface;

import java.nio.ByteBuffer;

public class MediaCodecController {

    private MediaFormat formatDecodeAudio;
    private MediaFormat formatDecodeVideo;
    private MediaFormat formatEncodeAudio;
    private MediaFormat formatEncodeVideo;

    private MediaCodec decoderAudio;
    private MediaCodec decoderVideo;
    private MediaCodec encoderAudio;
    private MediaCodec encoderVideo;

    private MediaCodec.BufferInfo outputBufferInfo;


    public Surface surface;


    private static MediaCodecController instance;

    private MediaCodecController() {

    }

    public synchronized static MediaCodecController getInstance() {
        if (instance == null) {
            instance = new MediaCodecController();
        }
        return instance;
    }

    public void initAudioDecoder() {

    }

    public void initVideoDecoder(String mimeType, int width, int height, byte[] csd_0, byte[] csd_1) {


        if (surface != null) {
            try {
                formatDecodeVideo = MediaFormat.createVideoFormat(mimeType, width, height);
                formatDecodeVideo.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, width * height);
                formatDecodeVideo.setByteBuffer("csd-0", ByteBuffer.wrap(csd_0));
                formatDecodeVideo.setByteBuffer("csd-1", ByteBuffer.wrap(csd_1));

                decoderVideo = MediaCodec.createDecoderByType(mimeType);

                outputBufferInfo = new MediaCodec.BufferInfo();

                decoderVideo.configure(formatDecodeVideo, surface, null, 0);

                decoderVideo.start();

            } catch (Exception e) {
                e.printStackTrace();
            }
        }


    }

    public void initAudioEncoder() {
    }

    public void initVideoEncoder() {
    }

    public void decodeVideo(byte[] data, int size) {

        if (surface != null && size > 0 && data != null && decoderVideo != null) {

            try {

                int inputBufferIndex = decoderVideo.dequeueInputBuffer(10);

                if (inputBufferIndex >= 0) {

                    ByteBuffer byteBuffer = decoderVideo.getInputBuffers()[inputBufferIndex];
                    byteBuffer.clear();
                    byteBuffer.put(data);
                    decoderVideo.queueInputBuffer(inputBufferIndex, 0, size, 10, 0);
                }

                int outputBufferIndex = decoderVideo.dequeueOutputBuffer(outputBufferInfo, 10);

                while (outputBufferIndex >= 0) {

                    decoderVideo.releaseOutputBuffer(outputBufferIndex, true);
                    outputBufferIndex = decoderVideo.dequeueOutputBuffer(outputBufferInfo, 10);
                }


            } catch (Exception e) {
                e.printStackTrace();
            }


        }

    }


    public void releaseCodec() {
        try {
            if (decoderAudio != null) {
                decoderAudio.flush();
                decoderAudio.stop();
                decoderAudio.release();
                decoderAudio = null;
            }

            if (decoderVideo != null) {
                decoderVideo.flush();
                decoderVideo.stop();
                decoderVideo.release();
                decoderVideo = null;
            }

            if (encoderAudio != null) {
                encoderAudio.flush();
                encoderAudio.stop();
                encoderAudio.release();
                encoderAudio = null;
            }

            if (encoderVideo != null) {
                encoderVideo.flush();
                encoderVideo.stop();
                encoderVideo.release();
                encoderVideo = null;
            }
            if (formatDecodeAudio != null)
                formatDecodeAudio = null;

            if (formatDecodeVideo != null)
                formatDecodeVideo = null;

            if (formatEncodeAudio != null)
                formatEncodeAudio = null;

            if (formatEncodeVideo != null)
                formatEncodeVideo = null;

        } catch (Exception e) {
            e.printStackTrace();
        }

    }

    private int getACCSampleRate(int sampleRate) {

        int rate = 4;
        switch (sampleRate) {
            case 96000:
                rate = 0;
                break;
            case 88200:
                rate = 1;
                break;
            case 64000:
                rate = 2;
                break;
            case 48000:
                rate = 3;
                break;
            case 44100:
                rate = 4;
                break;
            case 32000:
                rate = 5;
                break;
            case 24000:
                rate = 6;
                break;
            case 22050:
                rate = 7;
                break;
            case 16000:
                rate = 8;
                break;
            case 12000:
                rate = 9;
                break;
            case 11025:
                rate = 10;
                break;
            case 8000:
                rate = 11;
                break;
            case 7350:
                rate = 12;
                break;
        }
        return rate;
    }

}
