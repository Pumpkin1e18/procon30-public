using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

// Procon30

public class TimeSetScript : MonoBehaviour {

	Button btn;
	GameObject field;

	void Start () {
		btn = GetComponent<Button> ();
		btn.interactable = true;
		field = GameObject.Find ("Field");
	}

	public void OnClick(){
		Debug.Log ("Time Set");
		ScoreScript.timeFloat = 0;
	}

	void Update () {
		
	}
}
