
//
function fun_1_87(event){
    const vggSdk = new _currentVgg.VggSdk();

    if (vggSdk.setEnv) {
        vggSdk.setEnv(_currentVggEnv);
    }

    switch (event.type) {
        
        case "mouseenter":{
            vggSdk.presentState(event.target, "1:87", "1:105", {"animation": {"type": "none", "timingFunction": "linear", "duration": 0.15000000596046448}, "resetScrollPosition": false});
            break;
        }
        case "mouseleave":{
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

export default fun_1_87;
