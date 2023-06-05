using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class SwitchingScript : MonoBehaviour {

	Button btn;
	GameObject field;
	strAnalyze analysis;

	void Start () {
		btn = GetComponent<Button> ();
		btn.interactable = true;
		field = GameObject.Find ("Field");
		analysis = field.GetComponent<strAnalyze> ();
	}

	public void OnClick(){
		if (FieldCreate.switching == 0) {
			FieldCreate.switching = 1;
			Debug.Log ("Switched heap map!");
			analysis.dispHeatMap ();
		} else {
			FieldCreate.switching = 0;
			Debug.Log ("Switched game map!");
		}
	}

	void Update () {
		
	}
}
