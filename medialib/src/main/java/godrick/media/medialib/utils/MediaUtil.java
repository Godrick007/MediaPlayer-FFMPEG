package godrick.media.medialib.utils;

import android.media.MediaCodecList;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.os.ParcelFileDescriptor;

import java.io.File;
import java.io.IOException;

public class MediaUtil {


    public static String[] getMediaCodecSupport(String url) {

        String[] arr = new String[1024];

        MediaExtractor extractor = new MediaExtractor();

        ParcelFileDescriptor fd = null;

        int index = 0;

        try {


            fd = ParcelFileDescriptor.open(new File(url), ParcelFileDescriptor.MODE_READ_ONLY);

            extractor.setDataSource(fd.getFileDescriptor());

            for (int i = 0; i < extractor.getTrackCount(); i++) {

                MediaFormat mediaFormat = extractor.getTrackFormat(i);

                String mime = mediaFormat.getString(MediaFormat.KEY_MIME);

                if (checkMediaCodecSupport(mime)) {
                    arr[index++] = mime;
                }

            }

        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            extractor.release();
            try {
                if (fd != null) {
                    fd.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }


        return arr;
    }


    public static boolean checkMediaCodecSupport(String mime) {

        for (int i = 0; i < MediaCodecList.getCodecCount(); i++) {

            String[] ss = MediaCodecList.getCodecInfoAt(i).getSupportedTypes();

            for (int j = 0; j < ss.length; j++) {

                if (mime.equals(ss[j])) {
                    return true;
                }

            }


        }
        return false;
    }

}
