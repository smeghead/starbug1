var Starbug1Calendar = {
    Calendar: Class.create(),
    Util: Class.create()
}; 
Object.extend(Starbug1Calendar.Util, {
    DAY_OF_WEEK: ["日", "月", "火", "水", "木", "金", "土"],
    getAddedDate: function(date, n) {
        var d = new Date();
        d.setTime(date.getTime());
        d.setTime(d.getTime() + (1000 * 60 * 60 * 24 * n));
        return d;
    },
    getHeaderHtml: function() {
        return '<tr>' + this.DAY_OF_WEEK.inject(
            '',
            function(memo, val, i) {
                return memo + '<th class="day_' + i + '">' + val + '</th>';
            }
        ) + '</tr>';
    },
    getDaysHtml: function(date) {
        var firstDate = new Date(date.getYear(), date.getMonth(), 1);
        var nextFirstDate = new Date(date.getYear(), date.getMonth() + 1, 1);
        var currentDate = Starbug1Calendar.Util.getAddedDate(firstDate, firstDate.getDay() * -1);
        var endDate = Starbug1Calendar.Util.getAddedDate(nextFirstDate, 4 - firstDate.getDay());
        var buf = "";
        while (currentDate < endDate) {
            var dayOfWeek = currentDate.getDay();
            if (dayOfWeek == 0) buf += '<tr>';
            var className = 'day_' + dayOfWeek;
            if (currentDate.getMonth() != firstDate.getMonth()) {
                className += ' out';
            } else if (currentDate.getDate() == date.getDate()) {
                className += ' selected';
            }
            buf += '<td class="' + className + '">' + currentDate.getDate() + '</td>';
            if (dayOfWeek == 6) buf += '</tr>';
            currentDate = Starbug1Calendar.Util.getAddedDate(currentDate, 1);
        }
        return buf;
    }
});
Object.extend(Starbug1Calendar.Calendar.prototype, {
    date: null,
    target: null,
    calendar: document.createElement('div'),
    initialize: function(target, dateString) {
        this.date = new Date(dateString.replace("-", "/"));
        if (isNaN(this.date)) this.date = new Date();
        this.target = $(target);
        this.calendar = Object.extend(this.calendar, {
            id: 'calendar',
            innerHTML: '<table>' +
                '<tr class="title">' +
                    '<th id="pre">《</th>' +
                    '<th class="title" colspan="5">' + this.date.getFullYear() + '年' + (this.date.getMonth() + 1) + '月</th>' +
                    '<th id="next">》</th>' +
                '</td></tr>' +
                Starbug1Calendar.Util.getHeaderHtml(this.date) +
                Starbug1Calendar.Util.getDaysHtml(this.date) + '</table>'
        });
        //Point
        var offsets = Position.positionedOffset(this.target);
        this.calendar.style.posLeft = offsets[0];
        this.calendar.style.posTop = offsets[1] + this.target.style.height;
        //Event
        var _this = this;
        var selector = new Selector('td');
        selector.findElements(this.calendar).each(
            function(elem){
                if (elem.className.match('out')) return;
                Event.observe(elem, 'mouseover', 
                    function(){
                        elem.orgClassName = elem.className;
                        elem.className = 'onmouse';
                    }
                );
                Event.observe(elem, 'mouseout', 
                    function() {
                        elem.className = elem.orgClassName;
                    }
                );
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
//                 Event.observe('pre', 'click', 
//                     function() {
//                         var date = _this.date.getFullYear() + '-' + (_this.date.getMonth() - 1) + '-' + '01';
//                         _this = new Starbug1Calendar.Calendar(_this.target, date);
//                     }
//                 );
//                 Event.observe('next', 'click', 
//                     function() {
//                         var date = _this.date.getFullYear() + '-' + (_this.date.getMonth() + 1) + '-' + '01';
//                         _this = new Starbug1Calendar.Calendar(_this.target, date);
//                     }
//                 );
            }
        );
        this.target.parentNode.appendChild(this.calendar);
    },
    display: function() {
        Element.show(this.calendar);
    },
    hide: function() {
        Element.hide(this.calendar);
    }
});
Event.observe(window, "load", function() {
    var selector = new Selector('input[class=calendar]');
    selector.findElements().each(
        function(elem) {
            var cale;
            Event.observe(elem, 'focus', 
                function() {
                    cale = new Starbug1Calendar.Calendar(elem, $F(elem));
                    cale.display();
                }
            );
            Event.observe(elem, 'blur', 
                function() {
                    // 日付が選択された場合には、先に選択された処理が行なわれるようにする。
                    setTimeout(function() { if (cale) cale.hide() }, 300);
                }
            );
        }
    );
});

