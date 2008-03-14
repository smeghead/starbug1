if (typeof document.activeElement == "undefined") {
    Event.observe(document, "focus",
        function(e) {
        alert
            document.activeElement = (e.target.nodeType == Node.TEXT_NODE) ? e.target.parentNode : e.target;
        }, false);
}
var Starbug1Calendar = {
    Calendar: Class.create(),
    Util: Class.create()
}; 
Object.extend(Starbug1Calendar.Util, {
    DAY_OF_WEEK: ["日", "月", "火", "水", "木", "金", "土"],
    getAddedDate: function(date, n) {
        var d = new Date();
        d.setTime(date.getTime() + (1000 * 60 * 60 * 24 * n));
        return d;
    },
    createCalendar: function(date) {
        return '<table>' +
            '<tr class="title">' +
                '<th id="pre">《</th>' +
                '<th class="title" colspan="4">' + date.getFullYear() + '年' + (date.getMonth() + 1) + '月</th>' +
                '<th id="close">×</th>' +
                '<th id="next">》</th>' +
            '</td></tr>' +
            '<tr>' + this.DAY_OF_WEEK.inject(
                '',
                function(memo, val, i) {
                    return memo + '<th class="day_' + i + '">' + val + '</th>';
                }
            ) + '</tr>' +
            Starbug1Calendar.Util.getDaysHtml(date) + '</table>'
    },
    getFirstDate: function(date) {
        return new Date(date.getFullYear(), date.getMonth(), 1);
    },
    getDaysHtml: function(date) {
        var firstDate = Starbug1Calendar.Util.getFirstDate(date);
        var nextFirstDate = new Date(date.getFullYear(), date.getMonth() + 1, 1);
        var currentDate = Starbug1Calendar.Util.getAddedDate(firstDate, firstDate.getDay() * -1);
        var buf = "";
        while (1) {
            var dayOfWeek = currentDate.getDay();
            if (dayOfWeek == 0) buf += '<tr>';
            var className = 'day_' + dayOfWeek;
            if (currentDate.getMonth() != date.getMonth()) {
                className += ' out';
            } else if (currentDate.getDate() == date.getDate()) {
                className += ' selected';
            }
            buf += '<td class="' + className + '">' + currentDate.getDate() + '</td>';
            if (dayOfWeek == 6) buf += '</tr>';
            currentDate = Starbug1Calendar.Util.getAddedDate(currentDate, 1);
            if (dayOfWeek == 6 && currentDate > nextFirstDate)
                break;
        }
        return buf;
    },
    mouseover: function(e){
        var elem = Event.element(e);
        elem.orgClassName = elem.className;
        elem.className = 'onmouse';
    },
    mouseout: function(e) {
        var elem = Event.element(e);
        elem.className = elem.orgClassName;
    }
});
Object.extend(Starbug1Calendar.Calendar.prototype, {
    date: null,
    target: null,
    timer: null,
    calendar: document.createElement('div'),
    initialize: function(target) {
        this.target = $(target);
    },
    display: function(dateString) {
        if (this.timer) {
            clearTimeout(this.timer);
        }
        this.calendar.innerHTML = "";
        this.date = dateString;
        if (typeof(dateString) == 'string') {
            this.date = new Date(dateString.replace(/-/g, "/"));
        }
        if (isNaN(this.date)) this.date = new Date();
        this.calendar.id = 'calendar';
        this.calendar.innerHTML = Starbug1Calendar.Util.createCalendar(this.date);
        //Event
        var _this = this;
        var selector = new Selector('td');
        selector.findElements(this.calendar).each(
            function(elem){
                if (elem.className.match('out')) return;
                Event.observe(elem, 'mouseover', Starbug1Calendar.Util.mouseover);
                Event.observe(elem, 'mouseout', Starbug1Calendar.Util.mouseout);
                Event.observe(elem, 'click', 
                    function() {
                        var y = _this.date.getFullYear();
                        var m = _this.date.getMonth() + 1;
                        var d = Number(elem.innerHTML);
                        _this.target.value = y.toPaddedString(4) + '-' +
                            m.toPaddedString(2) + '-' +
                            d.toPaddedString(2);
                        _this.hide();
                        _this = undefined;
                    }
                );
            }
        );
        this.target.parentNode.appendChild(this.calendar);
        //Point
        var offsets = Position.positionedOffset(this.target);
        this.calendar.style.left = offsets[0];
        this.calendar.style.top = offsets[1] + Element.getHeight(this.target);
        Event.observe('pre', 'click', 
            function() {
                var firstDate = Starbug1Calendar.Util.getFirstDate(_this.date);
                var date = Starbug1Calendar.Util.getAddedDate(firstDate, -1); 
                date.setDate(_this.date.getDate());
                _this.display(date);
            }
        );
        Event.observe('pre', 'mouseover', Starbug1Calendar.Util.mouseover);
        Event.observe('pre', 'mouseout', Starbug1Calendar.Util.mouseout);
        Event.observe('next', 'click', 
            function() {
                var firstDate = Starbug1Calendar.Util.getFirstDate(_this.date);
                var date = Starbug1Calendar.Util.getAddedDate(firstDate, 32); 
                date.setDate(_this.date.getDate());
                _this.display(date);
            }
        );
        Event.observe('next', 'mouseover', Starbug1Calendar.Util.mouseover);
        Event.observe('next', 'mouseout', Starbug1Calendar.Util.mouseout);
        Event.observe('close', 'mouseover', Starbug1Calendar.Util.mouseover);
        Event.observe('close', 'mouseout', Starbug1Calendar.Util.mouseout);
        Event.observe('close', 'click', _this.hide.bindAsEventListener(_this));
        Element.show(this.calendar);
    },
    hide: function() {
        Element.hide(this.calendar);
    },
    will_hide: function() {
        var _this = this;
        // 日付が選択された場合には、先に選択された処理が行なわれるようにする。
        if (!this.timer)
            this.timer = setTimeout(function() { _this.hide(); }, 200);
// $('state_index').innerHTML += "setting:" + this.timer;
    }
});
Event.observe(window, "load", function() {
    var selector = new Selector('input[class=calendar]');
    selector.findElements().each(
        function(elem) {
            Event.observe(elem, 'click', 
                function() {
                    cale = new Starbug1Calendar.Calendar(elem);
                    elem.calendar = cale;
                    cale.display($F(elem));
                }
            );
            Event.observe(elem, 'blur', 
                function() {
                    if (elem.calendar) elem.calendar.will_hide();
                }
            );
        }
    );
});
//  vim: set ts=4 sw=4 sts=4 expandtab fenc=utf-8:
