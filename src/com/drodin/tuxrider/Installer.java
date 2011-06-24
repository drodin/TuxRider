package com.drodin.tuxrider;

import java.io.File;
import java.io.FileOutputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.view.ViewGroup.LayoutParams;
import android.view.WindowManager;
import android.widget.LinearLayout;

public class Installer extends Activity {

	private static LinearLayout mOverlayView = null;

	@Override
	protected void onCreate(Bundle icicle) {
		super.onCreate(icicle);

		getWindow().setFlags(
				WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,
				WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		if (getWindowManager().getDefaultDisplay().getHeight()>getWindowManager().getDefaultDisplay().getWidth())
			setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

		mOverlayView = new LinearLayout(getApplicationContext());
		mOverlayView.setLayoutParams(new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));
		mOverlayView.setBackgroundDrawable(getResources().getDrawable(R.drawable.splash));

		setContentView(mOverlayView,
				new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));
		
		new Thread(new Runnable() {
            public void run() {
            	InstallFiles();
            	startActivity(new Intent(getApplicationContext(), MainActivity.class));
            	finish();
            }
        }).start();

	}
	
	
	public void InstallFiles() {
		final AssetManager mAssetManager = getApplication().getResources().getAssets();

		try {
			String dir = NativeLib.DATA_DIR + "/";

			File fdir = new File( dir );
			if (!fdir.exists())
				fdir.mkdirs();

			ZipInputStream zs = new ZipInputStream(mAssetManager.open("files.zip", AssetManager.ACCESS_BUFFER));

			ZipEntry item;
			while( (item = zs.getNextEntry())!=null ) {

				if( item.isDirectory() ) {
					File newdir = new File( dir + item.getName() );
					if (!newdir.exists())
						newdir.mkdir();
				}
				else {
					File newfile = new File( dir + item.getName() );
					long filesize = item.getSize();
					if (newfile.exists() && newfile.length() == filesize)
						continue;
					byte[] tempdata = new byte[(int)filesize];
					int offset = 0;
					while (offset<filesize)
						offset += zs.read(tempdata, offset, (int)filesize-offset);
					zs.closeEntry();
					newfile.createNewFile();
					FileOutputStream fo = new FileOutputStream(newfile);
					fo.write(tempdata);
					fo.close();
				}
			}

			zs.close();   
		}
		catch(Exception e)
		{
			//noop
		}
	}

}
