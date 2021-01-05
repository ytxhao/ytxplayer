package com.ytx.ican.ytxplayer.utils;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Build;
import android.telephony.TelephonyManager;
import android.util.Base64;

import com.ytx.ican.media.player.pragma.YtxLog;

import java.io.UnsupportedEncodingException;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

public class PreferenceUtil {
	private static final String TAG = "PreferenceUtil";
	private static final String PROFILE_NAME =  "ytxProfile";
	private static final String TRANSFORMATION = "AES/CBC/PKCS5Padding";
	private static final String SECRET_KEY_HASH_TRANSFORMATION = "SHA-256";
	private static final String CHARSET = "UTF-8";
	private static final String DEFAULT_VALUE = "default";
	/**
	 * 初始化向量参数，AES 为16bytes. DES 为8bytes.
	 */
	private static final String INIT_VECTOR_KEY = "initVectorString";
	public static final int MODE_ENCRYPT_NONE = 0;
	public static final int MODE_ENCRYPT_ALL = 1;
	public static final int MODE_ENCRYPT_PARTIAL = 2;

	private Cipher cipherWriter;
	private Cipher cipherReader;

	private static PreferenceUtil mInstance;
	private Context mContext;
	private SharedPreferences mPref;
	private int encryptMode;

	private PreferenceUtil() {
	}

	public synchronized static void initInstance(Context context) {
		if(mInstance == null){
			mInstance = new PreferenceUtil();
		}
		mInstance.mContext = context;
		mInstance.encryptMode = MODE_ENCRYPT_NONE;
	}

	public synchronized static void initInstance(Context context, int encryptMode) {
		if(mInstance == null){
			mInstance = new PreferenceUtil();
		}
		mInstance.mContext = context;
		mInstance.encryptMode = encryptMode;
		if(encryptMode == MODE_ENCRYPT_ALL) {
			mInstance.initCiphers();
		}
	}

	public synchronized static PreferenceUtil getInstance() {
		return getInstance(PROFILE_NAME);
	}


	public synchronized static PreferenceUtil getInstance(String preferenceName) {
		mInstance.mPref = mInstance.mContext.getSharedPreferences(preferenceName, Context.MODE_PRIVATE);
		return mInstance;
	}

	private Editor getEditor(){
		return mPref.edit();
	}

	public void putString(String key, String value) {
		String convertValue = value;
		if(encryptMode == MODE_ENCRYPT_ALL) {
			convertValue = encrypt(value);
		}
		if(convertValue != null) {
			Editor editor = mPref.edit();
			editor.putString(key, convertValue);
			editor.commit();
		}
	}

	public void putLong(String key, long value) {
		if(encryptMode == MODE_ENCRYPT_ALL){
			putString(key, String.valueOf(value));
		}else {
			Editor editor = mPref.edit();
			editor.putLong(key, value);
			editor.commit();
		}
	}

	public void putInt(String key, int value) {
		if(encryptMode == MODE_ENCRYPT_ALL){
			putString(key, String.valueOf(value));
		}else {
			Editor editor = mPref.edit();
			editor.putInt(key, value);
			editor.commit();
		}
	}

	public void putBoolean(String key, boolean value) {
		if(encryptMode == MODE_ENCRYPT_ALL){
			putString(key, String.valueOf(value));
		}else {
			Editor editor = mPref.edit();
			editor.putBoolean(key, value);
			editor.commit();
		}
	}

	public String getString(String key) {
		return getString(key, "");
	}

	public String getString(String key, String defaultValue) {
		if(encryptMode != MODE_ENCRYPT_ALL)
			return mPref.getString(key, defaultValue);

		String value = mPref.getString(key, DEFAULT_VALUE);
		if(value.equals(DEFAULT_VALUE))//not exist
			return defaultValue;
		String retValue = decrypt(value);
		if(retValue == null)
			retValue = defaultValue;
		return retValue;
	}

	public int getInt(String key) {
		return getInt(key, 0);
	}

	public int getInt(String key, int defaultValue) {
		if(encryptMode != MODE_ENCRYPT_ALL)
			return mPref.getInt(key, defaultValue);

		String value = mPref.getString(key, DEFAULT_VALUE);
		if(value.equals(DEFAULT_VALUE))//not exist
			return defaultValue;
		String strValue = decrypt(value);
		if(strValue == null)//decrypt error
			return defaultValue;
		int intValue = defaultValue;
		try {
			intValue = Integer.parseInt(strValue);
		}catch (NumberFormatException e){
			e.printStackTrace();
		}

		return intValue;
	}

	public long getLong(String key) {
		return getLong(key, 0);
	}

	public long getLong(String key, long defaultValue) {
		if(encryptMode != MODE_ENCRYPT_ALL)
			return mPref.getLong(key, defaultValue);

		String value = mPref.getString(key, DEFAULT_VALUE);
		if(value.equals(DEFAULT_VALUE))//not exist
			return defaultValue;
		String strValue = decrypt(value);
		if(strValue == null)//decrypt error
			return defaultValue;
		long lValue = defaultValue;
		try {
			lValue = Long.parseLong(strValue);
		}catch (NumberFormatException e){
			e.printStackTrace();
		}

		return lValue;
	}

	public boolean getBoolean(String key) {
		return getBoolean(key, false);
	}

	public boolean getBoolean(String key, boolean defaultValue) {
		if(encryptMode != MODE_ENCRYPT_ALL)
			return mPref.getBoolean(key, defaultValue);

		String value = mPref.getString(key, DEFAULT_VALUE);
		if(value.equals(DEFAULT_VALUE))//not exist
			return defaultValue;
		String strValue = decrypt(value);
		if(strValue == null)//decrypt error
			return defaultValue;
		boolean bValue = defaultValue;
		try {
			bValue = Boolean.parseBoolean(strValue);
		}catch (NumberFormatException e){
			e.printStackTrace();
		}

		return bValue;
	}

	public boolean contains(String key) {
		return mPref.contains(key);
	}

	public void remove(String key) {
		Editor editor = mPref.edit();
		editor.remove(key);
		editor.commit();
	}

	public void clear() {
		Editor editor = mPref.edit();
		editor.clear();
		editor.commit();
	}

	private void initCiphers() {
		try {
			/**
			 *使用TRANSFORMATION创建一个加密器,一个解密器
			 */
			cipherWriter = Cipher.getInstance(TRANSFORMATION);
			cipherReader = Cipher.getInstance(TRANSFORMATION);
		} catch (NoSuchAlgorithmException e) {
			e.printStackTrace();
		} catch (NoSuchPaddingException e) {
			e.printStackTrace();
		}

		IvParameterSpec ivSpec = getIv(INIT_VECTOR_KEY);
		SecretKeySpec secretKey = getSecretKey();

		try {
			cipherWriter.init(Cipher.ENCRYPT_MODE, secretKey, ivSpec);
			cipherReader.init(Cipher.DECRYPT_MODE, secretKey, ivSpec);
		} catch (InvalidKeyException e) {
			e.printStackTrace();
		} catch (InvalidAlgorithmParameterException e) {
			e.printStackTrace();
		}
	}

	/**
	 * 创建一个 初始化向量参数表 对象
	 * @return
     */
	private IvParameterSpec getIv(String initVectorString) {
		byte[] iv = new byte[cipherWriter.getBlockSize()];
		YtxLog.d(TAG,"cipherWriter.getBlockSize() = "+cipherWriter.getBlockSize());
		System.arraycopy(initVectorString.getBytes(), 0, iv, 0, cipherWriter.getBlockSize());
		return new IvParameterSpec(iv);
	}

	private SecretKeySpec getSecretKey(){
		byte[] keyBytes = createKeyBytes(getUniqueID());
		return new SecretKeySpec(keyBytes, TRANSFORMATION);
	}

	private byte[] createKeyBytes(String key){
		byte[] keyBytes = null;
		try {
			//消息摘要
			MessageDigest md = MessageDigest.getInstance(SECRET_KEY_HASH_TRANSFORMATION);
			md.reset();
			keyBytes = md.digest(key.getBytes(CHARSET));
		} catch (Exception e) {
			e.printStackTrace();
		}

		YtxLog.d(TAG,"keyBytes="+keyBytes);
		return keyBytes;
	}

	private String getUniqueID(){
		TelephonyManager telephonyMgr = (TelephonyManager)mContext.getSystemService(Context.TELEPHONY_SERVICE);
		String strImei;

		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
			strImei = telephonyMgr.getImei(0);
		} else {
			strImei = telephonyMgr.getDeviceId(0);
		}
		return strImei + DEFAULT_VALUE;
	}

	protected synchronized String encrypt(String value){
		byte[] secureValue;
		String secureValueEncoded = null;
		try {
			secureValue = cipherWriter.doFinal(value.getBytes(CHARSET));
			secureValueEncoded = Base64.encodeToString(secureValue, Base64.NO_WRAP);
		} catch (Exception e) {
			e.printStackTrace();
			initCiphers();
		}

		return secureValueEncoded;
	}

	protected synchronized String decrypt(String securedEncodedValue) {
		String plainText = null;
		byte[] securedValue;
		byte[] value;

		try {
			securedValue = Base64.decode(securedEncodedValue, Base64.NO_WRAP);
			value = cipherReader.doFinal(securedValue);
			plainText = new String(value, CHARSET);
		} catch (Exception e) {
			e.printStackTrace();
			initCiphers();
		}

		return plainText;
	}
}
