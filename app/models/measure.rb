class Measure < ApplicationRecord

    scope :recent, -> {where('created_at >= :thirty_six_hours_ago', thirty_six_hours_ago: Time.now - 36.hours)}

    def self.temp_by_time

        find_by_sql(<<-SQL
            SELECT
            date_trunc('minute', created_at) AS date_time,
            temp_out,
            temp_in
            FROM measures
            GROUP BY date_time, temp_out, temp_in
            ORDER BY date_time
            SQL
          ).map do |row|
            [
                row['date_time'].strftime("%d %m %H:%M"),
                row.temp_out,
                row.temp_in,
            ]
          end
    end

end
