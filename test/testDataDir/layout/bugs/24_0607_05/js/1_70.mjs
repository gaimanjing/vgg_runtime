
//
function fun_1_70(event){
    const vggSdk = new _currentVgg.VggSdk();

    if (vggSdk.setEnv) {
        vggSdk.setEnv(_currentVggEnv);
    }

    switch (event.type) {
        
        case "click":{
            vggSdk.setState(event.target, "1:70", "1:87", {"animation": {"type": "none", "timingFunction": "linear", "duration": 0.15000000596046448}, "resetScrollPosition": false});
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

export default fun_1_70;
