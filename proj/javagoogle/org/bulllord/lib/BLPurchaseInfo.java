package org.bulllord.lib;
import android.os.Parcel;
import android.os.Parcelable;
import android.util.Log;
import org.json.JSONException;
import org.json.JSONObject;
import java.util.Date;
public class BLPurchaseInfo implements Parcelable
{
	public final String mResponseData;
	public final String mSignature;
	public final BLPurchaseData mPurchaseData;
	public BLPurchaseInfo(String responseData, String signature)
	{
		mResponseData = responseData;
		mSignature = signature;
		mPurchaseData = parseResponseDataImpl();
	}
	BLPurchaseData parseResponseDataImpl()
	{
		try
		{
			JSONObject json = new JSONObject(mResponseData);
			BLPurchaseData data = new BLPurchaseData();
			data.mOrderId = json.optString("orderId");
			data.mPackageName = json.optString("packageName");
			data.mProductId = json.optString("productId");
			long purchaseTimeMillis = json.optLong("purchaseTime", 0);
			data.mPurchaseTime = purchaseTimeMillis != 0 ? new Date(purchaseTimeMillis) : null;
			data.mPurchaseState = BLPurchaseState.values()[json.optInt("purchaseState", 1)];
			data.mDeveloperPayload = json.optString("developerPayload");
			data.mPurchaseToken = json.getString("purchaseToken");
			data.mAutoRenewing = json.optBoolean("autoRenewing");
			return data;
		}
		catch (JSONException e)
		{
			Log.e("GoogleIAB", "Failed to parse response data", e);
			return null;
		}
	}
	@Override
	public boolean equals(Object o)
	{
		if (this == o)
			return true;
		if (o == null || !(o instanceof BLPurchaseInfo))
			return false;
		BLPurchaseInfo other = (BLPurchaseInfo) o;
		return mResponseData.equals(other.mResponseData) && mSignature.equals(other.mSignature) && mPurchaseData.mPurchaseToken.equals(other.mPurchaseData.mPurchaseToken) && mPurchaseData.mPurchaseTime.equals(other.mPurchaseData.mPurchaseTime);
	}
	@Override
	public int describeContents()
	{
		return 0;
	}
	@Override
	public void writeToParcel(Parcel dest, int flags)
	{
		dest.writeString(mResponseData);
		dest.writeString(mSignature);
	}
	protected BLPurchaseInfo(Parcel in)
	{
		mResponseData = in.readString();
		mSignature = in.readString();
		mPurchaseData = parseResponseDataImpl();
	}
	public static final Parcelable.Creator<BLPurchaseInfo> CREATOR =
			new Parcelable.Creator<BLPurchaseInfo>()
			{
				public BLPurchaseInfo createFromParcel(Parcel source)
				{
					return new BLPurchaseInfo(source);
				}
				public BLPurchaseInfo[] newArray(int size)
				{
					return new BLPurchaseInfo[size];
				}
			};
}