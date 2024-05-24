
//
function fun_19_17(event){
    const vggSdk = new _currentVgg.VggSdk();

    if (vggSdk.setEnv) {
        vggSdk.setEnv(_currentVggEnv);
    }

    switch (event.type) {
        
        case "click":{
            vggSdk.openUrl("https://docs.verygoodgraphics.com/start/overview", "_blank");
            break;
        }
        case "mouseenter":{
            vggSdk.presentState(event.target, "19:17", "19:28", {"animation": {"type": "none", "timingFunction": "linear", "duration": 0.3}, "resetScrollPosition": false});
            break;
        }
        case "mouseleave":{
            vggSdk.dismissState(event.target);
            break;
        }
        case "mousedown":{
            vggSdk.presentState(event.target, "19:17", "19:34", {"animation": {"type": "none", "timingFunction": "linear", "duration": 0.3}, "resetScrollPosition": false});
            break;
        }
        case "mouseup":{
            vggSdk.dismissState(event.target);
            break;
        }

        default:
            break;
    }

    if (vggSdk.delete) {
        vggSdk.delete();
    }

    if (event.delete) {
        event.delete();
    }
}

export default fun_19_17;
