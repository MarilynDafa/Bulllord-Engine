package org.bulllord.lib;
import android.app.Activity;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.ComponentName;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Parcel;
import android.os.Parcelable;
import android.os.RemoteException;
import android.preference.PreferenceManager;
import android.text.TextUtils;
import android.util.Base64;
import android.util.Log;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collections;
import java.util.Date;
import java.util.List;
import java.util.Locale;
import java.util.UUID;
import java.util.HashMap;
import java.util.List;
import java.util.regex.Pattern;
import java.security.InvalidKeyException;
import java.security.KeyFactory;
import java.security.NoSuchAlgorithmException;
import java.security.PublicKey;
import java.security.Signature;
import java.security.SignatureException;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.X509EncodedKeySpec;
import com.android.vending.billing.IInAppBillingService;
import org.json.JSONObject;
import org.json.JSONException;
public class BLGoogleIAB
{
	public static final int BILLING_RESPONSE_RESULT_OK = 0;
	public static final int BILLING_RESPONSE_RESULT_ITEM_ALREADY_OWNED = 7;
	public static final int BILLING_ERROR_FAILED_LOAD_PURCHASES = 100;
	public static final int BILLING_ERROR_FAILED_TO_INITIALIZE_PURCHASE = 101;
	public static final int BILLING_ERROR_INVALID_SIGNATURE = 102;
	public static final int BILLING_ERROR_LOST_CONTEXT = 103;
	public static final int BILLING_ERROR_INVALID_MERCHANT_ID = 104;
	public static final int BILLING_ERROR_BIND_PLAY_STORE_FAILED = 113;	
	public static final int BILLING_ERROR_CONSUME_FAILED = 111;
	public static final int BILLING_ERROR_OTHER_ERROR = 110;
	private Context mContext;
	private String mSignatureBase64;
	private String mDeveloperPayload;
	private IBillingHandler mEventHandler;
	private String mContextPackageName;
	private BillingCache mCachedProducts;
	private IInAppBillingService mBillingService;		
	public interface IBillingHandler
	{
		void onProductPurchased(String productId, BLTransactionDetails details);
		void onPurchaseHistoryRestored();
		void onBillingError(int errorCode, Throwable error);
		void onBillingInitialized();
	}	
	private class BillingCache
	{
		private Context mContext;
		private HashMap<String, BLPurchaseInfo> mData;
		private String mCacheKey;
		private String mVersion;
		BillingCache(Context context, String key)
		{
			mContext = context;
			mData = new HashMap<String, BLPurchaseInfo>();
			mCacheKey = key;
			load();
		}
		private String getPreferencesCacheKey()
		{
			return mContext.getPackageName() + "_preferences" + mCacheKey;
		}
		private String getPreferencesVersionKey()
		{
			return getPreferencesCacheKey() + ".version";
		}
		private void load()
		{
			String[] entries;
			String tmp;
			SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(mContext);
			if (sp != null)
			{
				tmp = sp.getString(getPreferencesCacheKey(), "");
			}
			tmp = "";
			entries = tmp.split(Pattern.quote("#####"));
			for (String entry : entries)
			{
				if (!TextUtils.isEmpty(entry))
				{
					String[] parts = entry.split(Pattern.quote(">>>>>"));
					if (parts.length > 2)
					{
						mData.put(parts[0], new BLPurchaseInfo(parts[1], parts[2]));
					}
					else if (parts.length > 1)
					{
						mData.put(parts[0], new BLPurchaseInfo(parts[1], null));
					}
				}
			}
			mVersion = getCurrentVersion();
		}
		private void flush()
		{
			ArrayList<String> output = new ArrayList<String>();
			for (String productId : mData.keySet())
			{
				BLPurchaseInfo info = mData.get(productId);
				output.add(productId + ">>>>>" + info.mResponseData + ">>>>>" + info.mSignature);
			}
			SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(mContext);
			if (sp != null)
			{
				SharedPreferences.Editor spe = sp.edit();
				spe.putString(getPreferencesCacheKey(), TextUtils.join("#####", output));
				spe.commit();
			}
			mVersion = Long.toString(new Date().getTime());
			sp = PreferenceManager.getDefaultSharedPreferences(mContext);
			if (sp != null)
			{
				SharedPreferences.Editor spe = sp.edit();
				spe.putString(getPreferencesVersionKey(), mVersion);
				spe.commit();
			}
		}
		boolean includesProduct(String productId)
		{
			reloadDataIfNeeded();
			return mData.containsKey(productId);
		}
		BLPurchaseInfo getDetails(String productId)
		{
			reloadDataIfNeeded();
			return mData.containsKey(productId) ? mData.get(productId) : null;
		}
		void put(String productId, String details, String signature)
		{
			reloadDataIfNeeded();
			if (!mData.containsKey(productId))
			{
				mData.put(productId, new BLPurchaseInfo(details, signature));
				flush();
			}
		}
		void remove(String productId)
		{
			reloadDataIfNeeded();
			if (mData.containsKey(productId))
			{
				mData.remove(productId);
				flush();
			}
		}
		void clear()
		{
			reloadDataIfNeeded();
			mData.clear();
			flush();
		}
		private String getCurrentVersion()
		{
			SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(mContext);
			if (sp != null)
			{
				return sp.getString(getPreferencesVersionKey(), "0");
			}
			return "0";
		}
		private void reloadDataIfNeeded()
		{
			if (!mVersion.equalsIgnoreCase(getCurrentVersion()))
			{
				mData.clear();
				load();
			}
		}
		List<String> getContents()
		{
			return new ArrayList<String>(mData.keySet());
		}
		public String toString()
		{
			return TextUtils.join(", ", mData.keySet());
		}
	}	
	private class HistoryInitializationTask extends AsyncTask<Void, Void, Boolean>
	{
		@Override
		protected Boolean doInBackground(Void... nothing)
		{
			boolean ret;
			SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(mContext);
			if (sp != null)
				ret = sp.getBoolean(mContext.getPackageName() + "_preferences" + ".products.restored" + ".v2_6", false);
			else
				ret = false;
			if (!ret)
			{
				syncPurchases();
				return true;
			}
			return false;
		}
		@Override
		protected void onPostExecute(Boolean restored)
		{
			if (restored)
			{
				SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(mContext);
				if (sp != null)
				{
					SharedPreferences.Editor spe = sp.edit();
					spe.putBoolean(mContext.getPackageName() + "_preferences" + ".products.restored" + ".v2_6", true);
					spe.commit();
				}
				if (mEventHandler != null)
					mEventHandler.onPurchaseHistoryRestored();
			}
			if (mEventHandler != null)
				mEventHandler.onBillingInitialized();
		}
	}	
	private ServiceConnection mServiceConnection = new ServiceConnection()
	{
		@Override
		public void onServiceDisconnected(ComponentName name)
		{
			mBillingService = null;
		}
		@Override
		public void onServiceConnected(ComponentName name, IBinder service)
		{
			mBillingService = IInAppBillingService.Stub.asInterface(service);
			new HistoryInitializationTask().execute();
		}
	};	
	public BLGoogleIAB(Context context, IBillingHandler handler)
	{
		mContext = context.getApplicationContext();
		mEventHandler = handler;
		mContextPackageName = mContext.getPackageName();
		mCachedProducts = new BillingCache(mContext, ".products.cache" + ".v2_6");		
		try
		{
			Intent intent = new Intent("com.android.vending.billing.InAppBillingService.BIND");
			intent.setPackage("com.android.vending");
			mContext.bindService(intent, mServiceConnection, Context.BIND_AUTO_CREATE);
		}
		catch (Exception e)
		{
			Log.e("GoogleIAB", "error in bindPlayServices", e);
			mEventHandler.onBillingError(BILLING_ERROR_BIND_PLAY_STORE_FAILED, e);
		}
	}
	public void release()
	{
		if ((mBillingService != null) && mServiceConnection != null)
		{
			try
			{
				mContext.unbindService(mServiceConnection);
			}
			catch (Exception e)
			{
				Log.e("GoogleIAB", "Error in release", e);
			}
			mBillingService = null;
		}
	}	
	public void setPayload(String signature, String payload)
	{
		mSignatureBase64 = signature;
		mDeveloperPayload = payload;
	}
	public void checkIabServiceAvailable()
	{
		final PackageManager packageManager = mContext.getPackageManager();
		Intent intent = new Intent("com.android.vending.billing.InAppBillingService.BIND");
		intent.setPackage("com.android.vending");
		List<ResolveInfo> list = packageManager.queryIntentServices(intent, 0);
		if(list == null || list.size() == 0)
			Log.e("GoogleIAB", "IAB service unavailable");
	}
	public boolean purchase(Activity activity, String productId)
	{
		if (mBillingService == null || TextUtils.isEmpty(productId))
			return false;
		try
		{
			String purchasePayload = "inapp" + ":" + productId;
			purchasePayload += ":" + UUID.randomUUID().toString();
			purchasePayload += ":" + mDeveloperPayload;
			SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(mContext);
			if (sp != null)
			{
				SharedPreferences.Editor spe = sp.edit();
				spe.putString(mContext.getPackageName() + "_preferences" + ".purchase.last" + ".v2_6", purchasePayload);
				spe.commit();
			}			
			Bundle bundle = mBillingService.getBuyIntent(3, mContextPackageName, productId, "inapp", purchasePayload);
			if (bundle != null)
			{
				int response = bundle.getInt("RESPONSE_CODE");
				if (response == BILLING_RESPONSE_RESULT_OK)
				{
					PendingIntent pendingIntent = bundle.getParcelable("BUY_INTENT");
					if (activity != null && pendingIntent != null)
						activity.startIntentSenderForResult(pendingIntent.getIntentSender(), 32459, new Intent(), 0, 0, 0);
					else
						mEventHandler.onBillingError(BILLING_ERROR_LOST_CONTEXT, null);
				}
				else if (response == BILLING_RESPONSE_RESULT_ITEM_ALREADY_OWNED)
				{
					if (!mCachedProducts.includesProduct(productId))
						syncPurchases();
					BLTransactionDetails details;
					BLPurchaseInfo info = mCachedProducts.getDetails(productId);
					if (info != null && !TextUtils.isEmpty(info.mResponseData))
						details = new BLTransactionDetails(info);
					else
						details = null;		
					if (mEventHandler != null)
						mEventHandler.onProductPurchased(productId, details);
				}
				else
					mEventHandler.onBillingError(BILLING_ERROR_FAILED_TO_INITIALIZE_PURCHASE, null);
			}
			return true;
		}
		catch (Exception e)
		{
			Log.e("GoogleIAB", "Error in purchase", e);
			mEventHandler.onBillingError(BILLING_ERROR_OTHER_ERROR, e);
		}
		return false;
	}
	public boolean consumePurchase(String productId)
	{
		if (mBillingService == null)
			return false;
		try
		{
			BLTransactionDetails transaction;
			BLPurchaseInfo details = mCachedProducts.getDetails(productId);
			if (details != null && !TextUtils.isEmpty(details.mResponseData))
				transaction = new BLTransactionDetails(details);
			else
				transaction = null;		
			if (transaction != null && !TextUtils.isEmpty(transaction.mPurchaseInfo.mPurchaseData.mPurchaseToken))
			{
				int response = mBillingService.consumePurchase(3, mContextPackageName, transaction.mPurchaseInfo.mPurchaseData.mPurchaseToken);
				if (response == BILLING_RESPONSE_RESULT_OK)
				{
					mCachedProducts.remove(productId);
					Log.d("GoogleIAB", "Successfully consumed " + productId + " purchase.");
					return true;
				}
				else
				{
					mEventHandler.onBillingError(response, null);
					Log.e("GoogleIAB", String.format("Failed to consume %s: %d", productId, response));
				}
			}
		}
		catch (Exception e)
		{
			Log.e("GoogleIAB", "Error in consumePurchase", e);
			mEventHandler.onBillingError(BILLING_ERROR_CONSUME_FAILED, e);
		}
		return false;
	}	
	public boolean verifyPurchaseSignature(String productId, String purchaseData, String dataSignature)
	{
		if (TextUtils.isEmpty(mSignatureBase64))
			return true;
		else	
		{
			if (TextUtils.isEmpty(purchaseData) || TextUtils.isEmpty(mSignatureBase64) || TextUtils.isEmpty(dataSignature))
			{
				if (productId.equals("android.test.purchased") || productId.equals("android.test.canceled") || productId.equals("android.test.refunded") || productId.equals("android.test.item_unavailable"))
					return true;
				Log.e("GoogleIAB", "Purchase verification failed: missing data.");
				return false;
			}
			PublicKey key;		
			try
			{
				byte[] decodedKey = Base64.decode(mSignatureBase64, Base64.DEFAULT);
				KeyFactory keyFactory = KeyFactory.getInstance("RSA");
				key = keyFactory.generatePublic(new X509EncodedKeySpec(decodedKey));
			}
			catch (NoSuchAlgorithmException e)
			{
				throw new RuntimeException(e);
			}
			catch (InvalidKeySpecException e)
			{
				Log.e("GoogleIAB", "Invalid key specification.");
				throw new IllegalArgumentException(e);
			}
			catch (IllegalArgumentException e)
			{
				Log.e("GoogleIAB", "Base64 decoding failed.");
				throw e;
			}
			Signature sig;
			try
			{
				sig = Signature.getInstance("SHA1withRSA");
				sig.initVerify(key);
				sig.update(purchaseData.getBytes());
				if (!sig.verify(Base64.decode(dataSignature, Base64.DEFAULT)))
				{
					Log.e("GoogleIAB", "Signature verification failed.");
					return false;
				}
				return true;
			}
			catch (NoSuchAlgorithmException e)
			{
				Log.e("GoogleIAB", "NoSuchAlgorithmException.");
			}
			catch (InvalidKeyException e)
			{
				Log.e("GoogleIAB", "Invalid key specification.");
			}
			catch (SignatureException e)
			{
				Log.e("GoogleIAB", "Signature exception.");
			}
			catch (IllegalArgumentException e)
			{
				Log.e("GoogleIAB", "Base64 decoding failed.");
			}
			return false;
		}
	}	
	private boolean syncPurchases()
	{
		if (mBillingService == null)
			return false;
		try
		{
			Bundle bundle = mBillingService.getPurchases(3, mContextPackageName, "inapp", null);
			if (bundle.getInt("RESPONSE_CODE") == BILLING_RESPONSE_RESULT_OK)
			{
				mCachedProducts.clear();
				ArrayList<String> purchaseList = bundle.getStringArrayList("INAPP_PURCHASE_DATA_LIST");
				ArrayList<String> signatureList = bundle.getStringArrayList("INAPP_DATA_SIGNATURE_LIST");
				if (purchaseList != null)
				{
					for (int i = 0; i < purchaseList.size(); i++)
					{
						String jsonData = purchaseList.get(i);
						if (!TextUtils.isEmpty(jsonData))
						{
							JSONObject purchase = new JSONObject(jsonData);
							String signature = signatureList != null && signatureList.size() > i ? signatureList.get(i) : null;
							mCachedProducts.put(purchase.getString("productId"), jsonData, signature);
						}
					}
				}
				return true;
			}
		}
		catch (Exception e)
		{
			mEventHandler.onBillingError(BILLING_ERROR_FAILED_LOAD_PURCHASES, e);
			Log.e("GoogleIAB", "Error in loadPurchasesByType", e);
		}
		return false;
	}
	public boolean handleActivityResult(int requestCode, int resultCode, Intent data)
	{
		if (requestCode != 32459)
			return false;
		if (data == null)
		{
			Log.e("GoogleIAB", "handleActivityResult: data is null!");
			return false;
		}
		int responseCode = data.getIntExtra("RESPONSE_CODE", BILLING_RESPONSE_RESULT_OK);
		Log.d("GoogleIAB", String.format("resultCode = %d, responseCode = %d", resultCode, responseCode));
		if (resultCode == Activity.RESULT_OK && responseCode == BILLING_RESPONSE_RESULT_OK)
		{
			String purchaseData = data.getStringExtra("INAPP_PURCHASE_DATA");
			String dataSignature = data.getStringExtra("INAPP_DATA_SIGNATURE");
			try
			{
				JSONObject purchase = new JSONObject(purchaseData);
				String productId = purchase.getString("productId");
                if (verifyPurchaseSignature(productId, purchaseData, dataSignature))
				{
					String purchaseType = "inapp";
					BillingCache cache = mCachedProducts;
					cache.put(productId, purchaseData, dataSignature);
					if (mEventHandler != null)
						mEventHandler.onProductPurchased(productId, new BLTransactionDetails(new BLPurchaseInfo(purchaseData, dataSignature)));
				}
                else
                {
                    Log.e("GoogleIAB", "Public key signature doesn't match!");
                    mEventHandler.onBillingError(BILLING_ERROR_INVALID_SIGNATURE, null);
                }
			}
			catch (Exception e)
			{
				Log.e("GoogleIAB", "Error in handleActivityResult", e);
				mEventHandler.onBillingError(BILLING_ERROR_OTHER_ERROR, e);
			}
			SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(mContext);
			if (sp != null)
			{
				SharedPreferences.Editor spe = sp.edit();
				spe.putString(mContext.getPackageName() + "_preferences" + ".purchase.last" + ".v2_6", null);
				spe.commit();
			}
		}
		else
			mEventHandler.onBillingError(responseCode, null);
		return true;
	}
}